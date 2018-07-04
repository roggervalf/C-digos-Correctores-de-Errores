#include "mainwindow.h"
#include "ui_mainwindow.h"

int versiones[][3]={{20,20,200},    //1
                    {46,46,1864},   //2
                    {52,52,2440},   //3
                    {78,78,5768}    //4
                    };

const int WHOJA =9600;
const int HHOJA =13100;

const int versionini=0;

const int QR_NORTE = 0;
const int QR_ESTE = 1;
const int QR_SUR = 2;
const int QR_OESTE = 3;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    proceso=new QProcess(this);

    if(!ui->axWidget->setControl("Adobe PDF Reader"))
        QMessageBox::critical(this, "Error", "Asegurese que tiene Adobe Reader (y ActiveX) instalado!");
}

MainWindow::~MainWindow()
{
    delete proceso;
    delete ui;
}

void MainWindow::on_OpenFileButton_pressed()//el verdadero
{
    QString fileName =QFileDialog::getOpenFileName(
                this,
                tr("Open Input File"),
                QDir::currentPath(),
                tr("Files") + " (*.*)");
    if(QFile::exists(fileName))
    {
        ui->lineEditFile->setText(fileName);
    }
}

void MainWindow::on_Codificar_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(
                this,
                tr("Save PDF"),
                QDir::currentPath(),
                tr("Pdf Files (*.pdf);;All Files (*)"));
    if (fileName.isEmpty())
        return;
    else {
        if(QFile::exists(ui->lineEditFile->text()))
        {
            ui->lineEditSaveFile->setText(fileName);

            QFileInfo fi(ui->lineEditFile->text());
            QString ext = fi.completeSuffix();

            int tam_su=ext.length();

            int info_o;
            int info_c;

            const char* ch = ext.toStdString().c_str();//extension hasta 6 caracteres

            tam=ui->spinBoxTam->value();//tamaño de impresion
            int version_i,tipo_i,ladosim;
            version_i=versionini;//version
            tipo_i=4;//tipo

            version=ui->spinBoxVersion->value()-1;//version

            tipo=ui->spinBoxBits->value();//tipo

            ReedMuller *R=new ReedMuller(tipo_i);
            tam_cod=R->tam_c;
            info_c=versiones[version_i][2];//cantidad de modulos de informacion
            info_o=((info_c-8)/R->tam_c)*tipo_i+8;//cantidad de informacion original por simbolo
            espaciado=ui->spinBoxEspaciado->value();
            bool ok;
            llave=ui->lineEditKey->text().toLongLong(&ok,10);//Como decimal
            if(!ok)
                llave=0;
            qDebug()<<llave<<"Esta es la llave";
            ConvertirLlave();
            a_c=new QBitArray(info_c);
            a_a=new QBitArray(info_c);

            int cad_o[R->tam_m];
            int cad_r[R->tam_c];
            cadena_o = cad_o;
            cadena_r = cad_r;

            int n=0,lugar=0;
            qint64 a;

            for(int i=0;i<6;i++){//extension
                if(i<tam_su)
                    a=ch[i];
                else
                    a=' ';
                for(int j=0;j<8;j++){
                    cadena_o[n++]=(a%2);

                    a=a>>1;
                    if(n==R->tam_m){
                        n=0;
                        R->Codificacion(cadena_o,cadena_r);
                        PasarCodificado(lugar);
                        lugar+=R->tam_c;
                    }
                }
            }

            a=version;//Pasar version
            qDebug()<<"version"<<version;
            for(int j=0;j<4;j++){
                cadena_o[n++]=(a%2);
                a=a>>1;
            }
            n=0;
            R->Codificacion(cadena_o,cadena_r);
            PasarCodificado(lugar);
            lugar+=R->tam_c;

            a=tipo;//Pasar tipo
            for(int j=0;j<4;j++){
                cadena_o[n++]=(a%2);
                a=a>>1;
            }
            qDebug()<<"tipo"<<tipo;
            n=0;
            R->Codificacion(cadena_o,cadena_r);
            PasarCodificado(lugar);
            lugar+=R->tam_c;

            ReedMuller *Rn=new ReedMuller(tipo);
            info_c=versiones[version][2];//cantidad de modulos de informacion
            info_o=((info_c-8)/Rn->tam_c)*tipo+4;//cantidad de informacion original por simbolo

            QFile arch;
            arch.setFileName(ui->lineEditFile->text());
            qint64 cantidad;//cantidad de bits
            qint64 sobran;//bits que sobran

            arch.open(QIODevice::ReadOnly);

            qint64 len = arch.size();

            cantidad=8*len;
            qDebug()<<"tamaño"<<len;
            sobran=(cantidad%(info_o-4));
            a=sobran;
            qDebug()<<"sobran"<<sobran;
            for(int j=0;j<16;j++){//pasar los que sobran
                cadena_o[n++]=(a%2);
                //qDebug()<<cadena_o[n-1];
                a=a>>1;
                if(n==R->tam_m){
                    n=0;
                    R->Codificacion(cadena_o,cadena_r);
                    PasarCodificado(lugar);
                    lugar+=R->tam_c;
                }
            }
            n=0;
            LlenarCeros(lugar);

            DesordenarCodificado();

            ladosim=versiones[version][0]*tam;

            delete R;
            QImage imageninicial;
            GenerarSimbolo(imageninicial,versiones[version_i][0],versiones[version_i][1]);

            quint8 *data = new quint8[len];

            arch.read((char*)data, len);

            QPdfWriter pdf(fileName);

            qDebug()<<"tipo"<<tipo<<"info_c"<<info_c;

            a_a->resize(info_c);
            a_c->resize(info_c);

            int cad_o2[Rn->tam_m];
            int cad_r2[Rn->tam_c];
            cadena_o = cad_o2;
            cadena_r = cad_r2;

            int piePag=HHOJA-300;
            const qreal horizontalMarginMM = 12.0;
            const qreal verticalMarginMM = 13.0;
            QPagedPaintDevice::Margins margins;
            margins.left = margins.right = horizontalMarginMM;
            margins.top = margins.bottom = verticalMarginMM;
            pdf.setMargins(margins);

            QPainter pripainter;
            pripainter.begin(&pdf);

            pripainter.setPen(Qt::black);
            pripainter.setFont(QFont("Times", 10));

            pripainter.drawImage(0,0,imageninicial);

            int hoja=1;
            pripainter.drawText(0,piePag,QString::number(hoja++));
            tam=ui->spinBoxTam->value();
            tam_cod=Rn->tam_c;

            lugar=0;

            qDebug()<<"len "<<len<<n;
            int ho,ve;
            ho=ladosim+espaciado;
            ve=0;

            if((ho+ladosim)>WHOJA){
                ho=0;
                ve+=ladosim+espaciado;
            }
            for(int i=0;i<len;i++){
                a=data[i];
                for(int j=0;j<8;j++){
                    cadena_o[n++]=(a%2);
                    a=a>>1;
                    if(n==(Rn->tam_m)){
                        n=0;
                        Rn->Codificacion(cadena_o,cadena_r);
                        PasarCodificado(lugar);
                        lugar+=Rn->tam_c;
                        if(lugar==(a_c->size()-8)){
                            DesordenarCodificado();

                            lugar=0;
                            if((ve+ladosim)>HHOJA&&ho==0){
                                qDebug()<<"hojaaa "<<hoja;
                                ve=0;
                                pdf.newPage();
                                pripainter.drawText(0,piePag,QString::number(hoja++));
                            }
                            QImage imagenpdf;
                            qDebug()<<ho<<ve;
                            GenerarSimbolo(imagenpdf,versiones[version][0],versiones[version][1]);
                            pripainter.drawImage(ho,ve,imagenpdf);
                            ho+=ladosim+espaciado;
                            if((ho+ladosim)>WHOJA){
                                ho=0;
                                ve+=ladosim+espaciado;
                            }
                            qDebug()<<ho<<ve;
                        }
                    }
                }
            }

            if(lugar!=0||n!=0){
                qDebug()<<"hoja "<<hoja;
                if((ve+ladosim)>HHOJA&&ho==0){
                    qDebug()<<"hojaaa "<<hoja;
                    ve=0;
                    pdf.newPage();
                    pripainter.drawText(0,piePag,QString::number(hoja++));
                }
                if(n!=0){
                    while(n<Rn->tam_m)
                        cadena_o[n++]=0;
                    n=0;
                    Rn->Codificacion(cadena_o,cadena_r);
                    PasarCodificado(lugar);
                    lugar+=Rn->tam_c;
                }
                LlenarCeros(lugar);
                DesordenarCodificado();

                NumeroHoja(hoja++);
                QImage imagenpdf;
                GenerarSimbolo(imagenpdf,versiones[version][0],versiones[version][1]);
                pripainter.drawImage(ho,ve,imagenpdf);
            }

            delete a_a;
            delete a_c;
            delete R;
            cadena_r=NULL;
            cadena_o=NULL;
            pripainter.end();
            arch.close();
            delete llavebit;
        }
        else
        {
            QMessageBox::warning(this,
                                 tr("Warning"),
                                 QString(tr("Make sure %1 exists."))
                                 .arg(ui->lineEditFile->text()));
        }
    }
}

void MainWindow::on_pushButtonConvert_clicked()
{
    QString fileName=ui->lineEditSaveFile->text();

    if (fileName.isEmpty())
        return;
    else {
        if(QFile::exists(fileName))
        {
            QFileInfo fileNamepdf(ui->lineEditSaveFile->text());
            QString nombrebase=fileNamepdf.baseName();
            qDebug()<<fileNamepdf.absolutePath();

            QDir direc(fileNamepdf.absolutePath());
            direc.mkdir(nombrebase+"-imagenespdf");

            QDir direcimages(fileNamepdf.absolutePath()+"/"+nombrebase+"-imagenespdf");
            qDebug()<<direcimages.absolutePath();
            QByteArray sentencia;
            sentencia.append("magick convert -density 300 -depth 16 -quality 90 ");
            sentencia.append(fileNamepdf.absoluteFilePath());
            sentencia.append(" ");
            sentencia.append(direcimages.absolutePath()+"/"+nombrebase+".png\n");
            sentencia.append("exit\n\r");
            proceso->start("cmd.exe");

            proceso->write(sentencia);
            proceso->waitForFinished();
            proceso->close();
            ui->lineEditImages->setText(direcimages.absolutePath());
        }
    }
}

void MainWindow::on_Decode_clicked()
{
    QString directorioImages(ui->lineEditImages->text());
    if (directorioImages.isEmpty())
        return;
    else {
        if(QFile::exists(directorioImages))
        {
            QStringList filters;
            filters << "*.png";

            QDir direci(directorioImages+"/");
            qDebug()<<"Path"<<direci.absolutePath();
            direci.setNameFilters(filters);
            QStringList lista = direci.entryList();
            qDebug() << lista;

            int orden[lista.length()];

            QRegExp rx("\\D(\\d+)\.png");

            QDir direc(direci.absolutePath());
            if(ui->checkBoxImages->isChecked()){
                direc.mkdir("imagenesqr");
            }

            //QList<int> list;
            int pos = 0;

            for(int i=0;i<lista.length();i++){
                pos=rx.indexIn(lista[i],0);
                orden[i]=rx.cap(1).toInt(); // 0 1 10 11
                qDebug()<<orden[i];
            }

            QString archivofinal;
            QString extension("");
            int sobran;//,usimbolo;

            int info_c,info_o;
            quint8 *data;
            ReedMuller *R;
            int intermedio[8];
            int ite_iter=0,ite_data=0,banderaqr,banderaor;

            bool ok;
            llave=ui->lineEditKey->text().toLongLong(&ok,10);//Como decimal
            if(!ok)
                llave=0;
            qDebug()<<llave<<"Esta es la llave";
            ConvertirLlave();

            for(int hh=0;hh<lista.length();hh++){
                int numhoja;
                for(numhoja=0;orden[numhoja]!=hh;numhoja++){

                }
                QString hoja = direci.absolutePath()+"/"+lista[numhoja];
                qDebug()<<"hoja "<<hoja;

                cv::Mat image = cv::imread(hoja.toStdString());
                if(image.empty()){
                    qDebug() << "ERROR: No se puede encontrar la imagen.\n" << endl;
                    return;
                }

                cv::Mat qr_raw,nuevotam;
                int oractual=0;

                    banderaqr=ObtenerQR(image,qr_raw,banderaor);
                    if(banderaqr!=0){
                        if(banderaor!=oractual){
                            oractual=banderaor;
                        }
                        else{
                            if(hh==0){
                                version=versionini;
                                qDebug()<<version;
                                tipo=4;
                            }

                            info_c=versiones[version][2];//cantidad de modulos de informacion

                            tam=20;

                            //cv::Mat nuevotam;
                            qDebug()<<versiones[version][0]*tam<<versiones[version][1]*tam;
                            cv::resize(qr_raw, nuevotam,  cv::Size(versiones[version][0]*tam,versiones[version][1]*tam),0,0,cv::INTER_NEAREST);

                            if(ui->checkBoxImages->isChecked()){
                                QString nombre=direc.absolutePath()+"/imagenesqr/qr"+QString::number(hh)+".png";
                                cv::imwrite(nombre.toStdString(),nuevotam);//guardo imagen recortada
                            }
                            //informacion de la primera imagen
                            R=new ReedMuller(tipo);
                            tam_cod=R->tam_c;

                            qDebug()<<R->tam_c<<R->tam_m;//

                            info_o=((info_c-8)/R->tam_c)*tipo+4;//cantidad de informacion original por simbolo

                            if(hh==0){
                                a_a=new QBitArray(info_c);
                                a_c=new QBitArray(info_c);
                                a_o=new QBitArray(info_o);
                            }
                            else{
                                a_a->resize(info_c);
                                a_c->resize(info_c);
                                a_o->resize(info_o);
                            }

                            ObtenerDatos(versiones[version][0],versiones[version][1],nuevotam);

                            ReordenarCodificado();
                            //a_a=NULL;

                            int cad_o[R->tam_m];
                            int cad_r[R->tam_c];
                            cadena_o = cad_o;
                            cadena_r = cad_r;

                            int ch=0,lugar_c=0,lugar_o=0;

                            tam_men=R->tam_m;
                            for(int i=0;i<(info_c-8)/R->tam_c;i++){
                                for(int j=0;j<R->tam_c;j++){
                                    cadena_r[j]=ObtenerBitC(lugar_c++);
                                }
                                R->Correccion(cadena_r,cadena_o);
                                PasarDecodificado(lugar_o);
                                lugar_o+=R->tam_m;
                            }

                            if(hh==0){

                                for(int i=1;i<=6;i++){//extension
                                    for(int j=(i*8-1);j>=((i-1)*8);j--){
                                        ch+=ObtenerBitD(j);
                                        ch=ch<<1;
                                    }
                                    ch=ch>>1;
                                    if(ch!=' ')
                                        extension=extension+(char)ch;
                                    ch=0;
                                }
                                qDebug()<<extension;

                                archivofinal = QFileDialog::getSaveFileName(
                                            this,
                                            tr("Save Final File"),
                                            QDir::currentPath(),
                                            //tr("Files ")+
                                            "(*."+extension+")");

                                for(int j=51;j>=48;j--){//version
                                    ch+=ObtenerBitD(j);
                                    ch=ch<<1;
                                }
                                ch=ch>>1;
                                version=ch;
                                ch=0;
                                qDebug()<<"version"<<version;

                                for(int j=55;j>=52;j--){//tipo
                                    ch+=ObtenerBitD(j);
                                    ch=ch<<1;
                                }
                                ch=ch>>1;
                                tipo=ch;
                                ch=0;
                                qDebug()<<"tipo"<<tipo;

                                for(int j=71;j>=56;j--){//sobran
                                    ch+=ObtenerBitD(j);
                                    ch=ch<<1;
                                }
                                ch=ch>>1;

                                sobran=ch;
                                ch=0;

                                qDebug()<<"sobran"<<sobran;

                                delete R;
                                info_c=versiones[version][2];
                                R=new ReedMuller(tipo);

                                info_o=((info_c-8)/R->tam_c)*tipo+4;
                                int informacion;
                                if(sobran==0)
                                    informacion=(info_o-4)*(lista.length()-1);
                                else
                                    informacion=(info_o-4)*(lista.length()-2)+sobran;
                                qDebug()<<"bytes1"<<informacion%8;
                                qDebug()<<"bytesfinales"<<informacion/8;

                                if(!archivofinal.isEmpty())
                                    data= new quint8[informacion/8];
                                else{
                                    hh=lista.length();
                                    qDebug()<<"esta vacia";
                                }
                            }
                            else{
                                if(!archivofinal.isEmpty()){
                                    int j=0,medio,inicio=0,sobrantes;
                                    ch=0;
                                    qDebug()<<"ite_iter"<<ite_iter;
                                    if(ite_iter!=0){
                                        inicio=8-ite_iter;
                                        for(j=0;j<inicio;j++)
                                            intermedio[ite_iter++]=ObtenerBitD(j);
                                        ite_iter=0;
                                        for(int k=7;k>=0;k--){
                                            ch+=intermedio[k];
                                            ch=ch<<1;
                                        }
                                        ch=ch>>1;
                                        data[ite_data++]=ch;
                                    }
                                    sobrantes=(a_o->size()-4-inicio)%8;
                                    qDebug()<<"sobrantes"<<sobrantes;
                                    if(hh==(lista.length()-1))
                                        medio=(sobran-inicio)/8;
                                    else
                                        medio=(a_o->size()-4-inicio-sobrantes)/8;
                                    qDebug()<<"numero bytes"<<medio;
                                    ch=0;
                                    for(int i=1;i<=medio;i++){
                                        for(j=inicio+(i*8-1);j>=inicio+((i-1)*8);j--){
                                            ch+=ObtenerBitD(j);
                                            ch=ch<<1;
                                        }
                                        ch=ch>>1;
                                        data[ite_data++]=ch;
                                        ch=0;
                                    }
                                    if(sobrantes!=0){
                                        for(j=(a_o->size()-4-sobrantes);j<(a_o->size()-4);j++)
                                            intermedio[ite_iter++]=ObtenerBitD(j);
                                    }
                                }
                            }

                            qDebug()<<"datos"<<ite_data;

                        }
                    }
                    else{
                        qDebug()<<"no hay qr";
                    }
                    cv::imwrite("imagen.png",image);

            }
            if(!archivofinal.isEmpty()){
                QFile arch;
                arch.setFileName(archivofinal);

                arch.open(QIODevice::WriteOnly);
                //arch.write()
                arch.write((char*)data,ite_data);

                ite_data=0;

                arch.close();
            }

            delete a_a;
            delete a_c;
            delete a_o;
            delete R;
            delete llavebit;
            if(!(ui->checkBoxImages->isChecked()))
                direci.removeRecursively();
        }
    }
}

int MainWindow::ObtenerQR(cv::Mat& image, cv::Mat& qr_raw, int& orientation){
    //Creacion de las imagenes para reconocer
    cv::Mat gray(image.size(), CV_MAKETYPE(image.depth(), 1));          // Imagen para la escala de grises
    cv::Mat edges(image.size(), CV_MAKETYPE(image.depth(), 1));			// Imagen que contiene los bordes

    std::vector<std::vector<cv::Point> > contours;  //Para almacenar los contornos
    std::vector<cv::Vec4i> hierarchy;               //Para almacenar la jeraquia de los contornos

    int mark,A,B,C,superior,derecha,inferior,median1,median2,fuera;
    float AB,BC,CA, dist,pendiente;

    int align;

    cv::cvtColor(image,gray,CV_RGB2GRAY);		// Convertir imagen a escala de grises

    double thresh = 180;
    double maxValor = 255;

    cv::blur( gray, gray, cv::Size(3,3) );

    // Binary Threshold
    cv::threshold(gray,gray, thresh, maxValor, cv::THRESH_BINARY);

    cv::Canny(gray, edges, 100 , 200, 3);		// Aplicar Canny para la deteccion de bordes en
                                                // la imagen con escala de grises

    cv::findContours( edges, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE); // Encontrar los contornos con jerarquia

    mark = 0;                                   // Resetear el contador de los marcadores

    // Obtener los momentos de todos los contornos con su centro de masa
    std::vector<cv::Moments> mu(contours.size());
    std::vector<cv::Point2f> mc(contours.size());

    for( uint i = 0; i < contours.size(); i++ ){
        mu[i] = cv::moments( contours[i], false );//momento
        mc[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );//centro de masa
    }

    // Empieza el procesamiento de los contornos

    // Encontrar los 3 contornos repetidos A,B,C
    // NOTA: 1. El contorno que encierra otros contornos se supone que son las tres marcas de Alineación del código QR.
    // 2. Alternativamente, la relación de áreas de los cuadrados "concéntricos" también se puede usar para identificar
    // los marcadores de alineación base. El siguiente demuestra el primer método
    for( uint i = 0; i < contours.size(); i++ ){
        int k=i;
        int c=0;

        while(hierarchy[k][2] != -1){
            k = hierarchy[k][2] ;
            c = c+1;
        }

        if (c >= 5)
        {
            if (mark == 0)		A = i;
            else if  (mark == 1)	B = i;
            else if  (mark == 2)	C = i;
            mark = mark + 1 ;
        }
    }

    qDebug()<<"marcadores"<<mark;
    if (mark >= 3)		// Asegurar que tengamos almenos 3 marcadores (A,B,C)
    {
        //Determinar el marcador superior de la izquierda
        //El vértice del triángulo no involucrado en el lado más largo es el 'valor atípico'

        AB = Distancia(mc[A],mc[B]);//centro de masa
        BC = Distancia(mc[B],mc[C]);
        CA = Distancia(mc[C],mc[A]);

        if ( AB > BC && AB > CA )
        {
            fuera = C; median1=A; median2=B;
        }
        else if ( CA > AB && CA > BC )
        {
            fuera = B; median1=A; median2=C;
        }
        else if ( BC > AB && BC > CA )
        {
            fuera = A;  median1=B; median2=C;
        }

        superior = fuera;							//  Marcador del tope izquierdo

        dist = DistEcuacionRecta(mc[median1], mc[median2], mc[fuera]);	// Obtener la distancia perpendicular del fuera con respecto al lado mas largo
        pendiente = PendienteRecta(mc[median1], mc[median2],align);		// Calcule también la pendiente del lado más largo

        // Ahora que tenemos la orientación de la línea formada por median1 y median2 y también tenemos la posición del valor fuera con la recta
        // Determinamos los marcadores 'derecho' e 'inferior'

        if (align == 0 && dist > 0)
        {
            derecha = median1;
            inferior = median2;
        }
        else if (align == 0 && dist < 0)
        {
            inferior = median1;
            derecha = median2;
        }
        else if (pendiente < 0 && dist < 0 )		// Orientacion - Norte
        {
            inferior = median1;
            derecha = median2;
            orientation = QR_NORTE;
        }
        else if (pendiente > 0 && dist < 0 )		// Orientacion - Este
        {
            derecha = median1;
            inferior = median2;
            orientation = QR_ESTE;
        }
        else if (pendiente < 0 && dist > 0 )		// Orientacion - Sur
        {
            derecha = median1;
            inferior = median2;
            orientation = QR_SUR;
        }
        else if (pendiente > 0 && dist > 0 )		// Orientacion - Oeste
        {
            inferior = median1;
            derecha = median2;
            orientation = QR_OESTE;
        }
        // Para garantizar que los valores no deseados no se escabullan cuando el código QR no está presente

        if( superior < contours.size() && derecha < contours.size() && inferior < contours.size() && cv::contourArea(contours[superior]) > 10 && cv::contourArea(contours[derecha]) > 10 && cv::contourArea(contours[inferior]) > 10 )
        {

            std::vector<cv::Point2f> L,M,O, tempL,tempM,tempO;
            cv::Point2f N;

            std::vector<cv::Point2f> src,dst;		// src - Los puntos de la imagen QR (sus 4 coordenadas)
                                                    // dst - Para el destino luego de su transformacion

            cv::Mat warp_matrix;

            ObtenerVertices(contours,superior,pendiente,tempL);
            ObtenerVertices(contours,derecha,pendiente,tempM);
            ObtenerVertices(contours,inferior,pendiente,tempO);

            ActualizarOrientacionEsquinas(orientation, tempL, L);
            ActualizarOrientacionEsquinas(orientation, tempM, M);
            ActualizarOrientacionEsquinas(orientation, tempO, O);

            cv::Point2f d1,d2;
            d1=M[1]-L[0];
            d2=O[3]-L[0];
            int iflag = ObtenerInterseccionPuntos(M[1],M[1]+d2,O[3],O[3]+d1,N);

            src.push_back(L[0]);
            src.push_back(M[1]);
            src.push_back(N);
            src.push_back(O[3]);

            double h1,h2,w1,w2;
            int h,w;

            w1=Distancia(src[0],src[1]);//ancho de arriba
            w2=Distancia(src[2],src[3]);//ancho de abajo
            w=(int)w1;
            if(w2>w) w=(int)w2;

            h1=Distancia(src[0],src[3]);//altura de la izquierda
            h2=Distancia(src[1],src[2]);//altura de la derecha
            h=(int)h1;
            if(h2>h) h=(int)h2;

            qDebug()<<"w"<<w;
            qDebug()<<"h"<<h;

            dst.push_back(cv::Point2f(0,0));
            dst.push_back(cv::Point2f(w,0));
            dst.push_back(cv::Point2f(w, h));
            dst.push_back(cv::Point2f(0, h));


            if (src.size() == 4 && dst.size() == 4 )//asegurar que solo se tenga 4 puntos para src y dst
            {
                qDebug()<<"entra";
                warp_matrix = cv::getPerspectiveTransform(src, dst);
                qDebug()<<w<<h;
                cv::warpPerspective(gray, qr_raw, warp_matrix, cv::Size(w, h));
            }
            return 1;
        }
    }
    else
        return 0;
}

QImage MainWindow::PatronLocalizar()
{
    QImage imagen(7*tam,7*tam,QImage::Format_Grayscale8);
    imagen.fill(Qt::black);
    QPainter painter(&imagen);

    QColor fg("black");
    QColor bg("white");

    painter.setBrush(bg);
    painter.setPen(Qt::NoPen);
    QRectF bn(tam,tam, 5*tam,5*tam);
    painter.drawRects(&bn,1);

    painter.setBrush(fg);
    QRectF bb(2*tam, 2*tam, 3*tam,3*tam);
    painter.drawRects(&bb,1);

    return imagen;
}

int MainWindow::Version(double r)
{
    int i=0;
    while(((r/versiones[i][0])>1.025)&&i<11){
        i++;
    }
    return i;
}

void MainWindow::PasarCodificado(int l)
{
    for(int i=0;i<tam_cod;i++){
        if(cadena_r[i]==0)
            a_c->clearBit(l++);
        else
            a_c->setBit(l++);
    }
}

void MainWindow::DesordenarCodificado()
{
    int ite=0;
    int tam_parte=a_c->size()/8;
    int l=0;
    for(int i=0;i<tam_parte;i++){
        for(int j=0;j<8;j++){
            if(llavebit[l++])
                a_c->toggleBit(ite);
            if(a_c->testBit(ite++))//1 blanco
                a_a->setBit(j*tam_parte+i);
            else
                a_a->clearBit(j*tam_parte+i);
            l=l%64;
        }
    }
}

void MainWindow::ReordenarCodificado()
{
    int ite=0;
    int tam_parte=a_c->size()/8;
    int l=0;
    for(int i=0;i<tam_parte;i++){
        for(int j=0;j<8;j++){
            if(a_a->testBit(j*tam_parte+i))//1 blanco
                a_c->setBit(ite++);
            else
                a_c->clearBit(ite++);
            if(llavebit[l++])
                a_c->toggleBit(ite-1);
            l=l%64;
        }
    }
}

void MainWindow::PasarDecodificado(int l)
{
    for(int i=0;i<tam_men;i++){
        if(cadena_o[i]==0)
            a_o->clearBit(l++);
        else
            a_o->setBit(l++);
    }
}

void MainWindow::LlenarCeros(int l)
{
    for(int i=l;i<a_c->size();i++){
        a_c->clearBit(i);
    }
}

void MainWindow::NumeroHoja(int h)
{
/**/
}

void MainWindow::ObtenerDatos(int modulosw, int modulosh,cv::Mat& image){

    int bit;
    qDebug()<<image.cols<<image.rows<<endl;
    int nn=0;

    int lugar=0;

    int ite=0,ini,tope,fil=8,col=8;
    for(int i=0;i<modulosh*tam;){
        if(i<(8*tam)||i>=((modulosh-8)*tam)){
            ini=8*tam;
        }
        else
            ini=0;
        if(i<(8*tam))
            tope=(modulosw-8)*tam;
        else
            tope=modulosw*tam;
        if(6==(i/tam)){
            i+=tam;
        }
        for(int j=ini;j<tope;){
            if(6==(j/tam)){
            j+=tam;
        }

        bit=ObtenerModulo(j, i,image);
        if(bit==0)
            a_a->clearBit(lugar++);
        else
            a_a->setBit(lugar++);
        j+=tam;
    }
    i+=tam;
}
}

int MainWindow::ObtenerModulo(int col,int fila, cv::Mat &image){
    double acumulador=0;

    for (int i = fila; i < fila+(tam); i++) {
        for (int j = col; j < col+(tam); j++) {
            cv::Scalar color = image.at<uchar>(cv::Point(j, i));
            acumulador += color.val[0];
        }
    }
    acumulador/=(tam*tam);
    if(acumulador<127)
        return 0;
    else
        return 1;
}

int MainWindow::ObtenerBitC(int lugar)
{
    if(a_c->testBit(lugar)){
        return 1;
    }
    else{
        return 0;
    }
}

int MainWindow::ObtenerBitD(int lugar)
{
    if(a_o->testBit(lugar)){
        return 1;
    }
    else{
        return 0;
    }
}

void MainWindow::GenerarSimbolo(QImage& imagen, int modulosw, int modulosh)
{
    imagen=QImage(modulosw*tam,modulosh*tam,QImage::Format_Grayscale8);

    imagen.fill(Qt::white);
    QPainter painter(&imagen);
    painter.setPen(Qt::NoPen);

    QColor negro("black");
    QColor blanco("white");

    int ite=0,ini,tope,fil=8,col=8;
    for(int i=0;i<modulosh*tam;){//filas
        if(i<(8*tam)||i>=((modulosh-8)*tam)){
            ini=8*tam;
        }
        else
            ini=0;
        if(i<(8*tam))
            tope=(modulosw-8)*tam;
        else
            tope=modulosw*tam;
        if(6==(i/tam)){
            for(;(col*tam)<tope;col++){
                if(col%2==1){
                    painter.setBrush(blanco);
                }
                else
                    painter.setBrush(negro);
                QRectF m(col*tam,i , tam,tam);
                painter.drawRects(&m,1);

            }
            i+=tam;
        }
        for(int j=ini;j<tope;){//columna
            if(6==(j/tam)){
                if(fil%2==0){
                    painter.setBrush(blanco);
                }
                else
                    painter.setBrush(negro);
                QRectF m(j,fil*tam , tam,tam);//(x,y) columna, fila
                painter.drawRects(&m,1);
                fil++;
                j+=tam;
            }

            if(a_a->testBit(ite++)){//1 blanco
                painter.setBrush(blanco);
            }
            else{//0 negro
                painter.setBrush(negro);
            }

            QRectF m(j, i, tam,tam);
            painter.drawRects(&m,1);
            j+=tam;
        }
        i+=tam;
    }
    QImage pl=PatronLocalizar();
    painter.drawImage(QPoint(0,0),pl);
    painter.drawImage(QPoint((modulosw-7)*tam,0),pl);
    painter.drawImage(QPoint(0,(modulosh-7)*tam),pl);
}

void MainWindow::on_pushButtonOpenPdf_clicked()
{
    QString fileName =QFileDialog::getOpenFileName(
                this,
                tr("Open Input File"),
                QDir::currentPath(),
                tr("Files ") + " (*.pdf)");
    if(QFile::exists(fileName))
    {
        ui->lineEditSaveFile->setText(fileName);
    }
}

// Se obtiene la distancia entre 2 puntos
float MainWindow::Distancia(cv::Point2f P, cv::Point2f Q)
{
    return sqrt(pow(abs(P.x - Q.x),2) + pow(abs(P.y - Q.y),2)) ;
}

// Se obtiene la distancia perpendicular del punto J hacia la recta formada por los puntos L y M
float MainWindow::DistEcuacionRecta(cv::Point2f L, cv::Point2f M, cv::Point2f J)
{
    float a,b,c,pdist;

    if((M.x - L.x)!=0){//No dividir por 0
        a = -((M.y - L.y) / (M.x - L.x));
        b = 1.0;
        c = (((M.y - L.y) /(M.x - L.x)) * L.x) - L.y;
        // ax + by + c, sustituyendo (x,y) por los valores del punto J

        pdist = (a * J.x + (b * J.y) + c)/sqrt((a * a) + (b * b));
    }
    else{
        pdist=J.x-M.x;
    }

    return pdist;
}

//Se obtiene la pendiente de la recta formada por los puntos L y M, ademas del alineamiento por si se va al infinito
float MainWindow::PendienteRecta(cv::Point2f L, cv::Point2f M, int& alineamiento)
{
    float dx,dy;
    dx = M.x - L.x;
    dy = M.y - L.y;

    if ( dy != 0)
    {
        alineamiento = 1;
        return (dy / dx);
    }
    else				// Asegurarse de no dividir por 0
    {
        alineamiento = 0;
        return 0.0;
    }
}

// Calcular las 4 esquinas de los marcadores
void MainWindow::ObtenerVertices(std::vector<std::vector<cv::Point> > contours, int c_id, float pendiente, std::vector<cv::Point2f>& quad)
{
    cv::Rect box;
    box = cv::boundingRect( contours[c_id]);

    cv::Point2f M0,M1,M2,M3;
    cv::Point2f A, B, C, D, W, X, Y, Z;

    A =  box.tl();
    B.x = box.br().x;
    B.y = box.tl().y;
    C = box.br();
    D.x = box.tl().x;
    D.y = box.br().y;

    W.x = (A.x + B.x) / 2;
    W.y = A.y;

    X.x = B.x;
    X.y = (B.y + C.y) / 2;

    Y.x = (C.x + D.x) / 2;
    Y.y = C.y;

    Z.x = D.x;
    Z.y = (D.y + A.y) / 2;

    float dmax[4];
    dmax[0]=0.0;
    dmax[1]=0.0;
    dmax[2]=0.0;
    dmax[3]=0.0;

    float pd1 = 0.0;
    float pd2 = 0.0;

    if (pendiente > 5 || pendiente < -5 )
    {
        for( uint i = 0; i < contours[c_id].size(); i++ )
        {
            pd1 = DistEcuacionRecta(C,A,contours[c_id][i]);	// Posicion del punto a AC
            pd2 = DistEcuacionRecta(B,D,contours[c_id][i]);	// Posicion del punto a BD

            if((pd1 >= 0.0) && (pd2 > 0.0))
            {
                ActualizarEsquina(contours[c_id][i],W,dmax[1],M1);
            }
            else if((pd1 > 0.0) && (pd2 <= 0.0))
            {
                ActualizarEsquina(contours[c_id][i],X,dmax[2],M2);
            }
            else if((pd1 <= 0.0) && (pd2 < 0.0))
            {
                ActualizarEsquina(contours[c_id][i],Y,dmax[3],M3);
            }
            else if((pd1 < 0.0) && (pd2 >= 0.0))
            {
                ActualizarEsquina(contours[c_id][i],Z,dmax[0],M0);
            }
            else
                continue;
        }
    }
    else
    {
        int halfx = (A.x + B.x) / 2;
        int halfy = (A.y + D.y) / 2;

        for( uint i = 0; i < contours[c_id].size(); i++ )
        {
            if((contours[c_id][i].x < halfx) && (contours[c_id][i].y <= halfy))
            {
                ActualizarEsquina(contours[c_id][i],C,dmax[2],M0);
            }
            else if((contours[c_id][i].x >= halfx) && (contours[c_id][i].y < halfy))
            {
                ActualizarEsquina(contours[c_id][i],D,dmax[3],M1);
            }
            else if((contours[c_id][i].x > halfx) && (contours[c_id][i].y >= halfy))
            {
                ActualizarEsquina(contours[c_id][i],A,dmax[0],M2);
            }
            else if((contours[c_id][i].x <= halfx) && (contours[c_id][i].y > halfy))
            {
                ActualizarEsquina(contours[c_id][i],B,dmax[1],M3);
            }
        }
    }

    quad.push_back(M0);
    quad.push_back(M1);
    quad.push_back(M2);
    quad.push_back(M3);
}

//Compara si un punto esta mas lejos que otro, si lo es, lo guarda
void MainWindow::ActualizarEsquina(cv::Point2f P, cv::Point2f ref , float& distlarga,  cv::Point2f& esquina)
{
    float temp_dist;
    temp_dist = Distancia(P,ref);

    if(temp_dist > distlarga)
    {
        distlarga = temp_dist;              // se actualiza la distancia mas grande
        esquina = P;						// P punto mas lejano
    }
}

void MainWindow::ActualizarOrientacionEsquinas(int orientacion, std::vector<cv::Point2f> in,std::vector<cv::Point2f> &out)
{
    cv::Point2f M0,M1,M2,M3;
    if(orientacion == QR_NORTE){
        M0 = in[0];
        M1 = in[1];
        M2 = in[2];
        M3 = in[3];
    }
    else if (orientacion == QR_ESTE)
    {
        M0 = in[1];
        M1 = in[2];
        M2 = in[3];
        M3 = in[0];
    }
    else if (orientacion == QR_SUR)
    {
        M0 = in[2];
        M1 = in[3];
        M2 = in[0];
        M3 = in[1];
    }
    else if (orientacion == QR_OESTE)
    {
        M0 = in[3];
        M1 = in[0];
        M2 = in[1];
        M3 = in[2];
    }
    out.push_back(M0);
    out.push_back(M1);
    out.push_back(M2);
    out.push_back(M3);
}

bool MainWindow::ObtenerInterseccionPuntos(cv::Point2f a1, cv::Point2f a2, cv::Point2f b1, cv::Point2f b2, cv::Point2f& intersection)
{
    cv::Point2f p = a1;
    cv::Point2f q = b1;
    cv::Point2f r(a2-a1);
    cv::Point2f s(b2-b1);

    if(cross(r,s) == 0) {intersection=a2;
        return false;}

    float t = cross(q-p,s)/cross(r,s);

    intersection = p + t*r;
    return true;
}

float MainWindow::cross(cv::Point2f v1,cv::Point2f v2)
{
    return v1.x*v2.y - v1.y*v2.x;
}

void MainWindow::ConvertirLlave()
{
    llavebit=new qint8[64];
    qint64 a=llave;
    for(int i=0;i<64;i++){
        llavebit[i]=(a%2);
        a=a>>1;
    }
}

void MainWindow::on_pushButtonOpenImages_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(
                this,
                tr("Open Directory"),
                QDir::currentPath(),
                QFileDialog::ShowDirsOnly
                | QFileDialog::DontResolveSymlinks);

    if(QFile::exists(dir))
    {
        ui->lineEditImages->setText(dir);
    }
}

void MainWindow::on_pushButtonVer_clicked()
{
    QString file = ui->lineEditSaveFile->text();
    if(QFile::exists(file))
    {
        ui->axWidget->dynamicCall("LoadFile(QString)",
                         QDir::toNativeSeparators(file));
    }
}
