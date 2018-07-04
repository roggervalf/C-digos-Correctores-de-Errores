#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileInfoList>
#include <QBitArray>
#include <QImage>
#include <QGraphicsItem>
#include <QPainter>
#include <QtPrintSupport/QPrinter>
#include <QPdfWriter>
#include <QPagedPaintDevice>
#include <QtMath>
#include <QProcess>
#include <ActiveQt>
#include <QRegExp>
#include <QGraphicsScene>

#include "opencv2/opencv.hpp"
#include "codigos.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_OpenFileButton_pressed();
    void on_Codificar_clicked();
    void on_Decode_clicked();
    void on_pushButtonOpenPdf_clicked();
    void on_pushButtonConvert_clicked();
    void on_pushButtonOpenImages_clicked();
    void on_pushButtonVer_clicked();

private:
    void PasarCodificado(int l);
    void DesordenarCodificado();
    void ReordenarCodificado();
    void PasarDecodificado(int l);
    void LlenarCeros(int l);
    void NumeroHoja(int h);
    void GenerarSimbolo(QImage& imagen, int modulosw, int modulosh);

    //QImage GenerarSimbolo(int modulosw, int modulosh);
    QImage PatronLocalizar();

    int Version(double r);
    void ObtenerDatos(int modulosw,int modulosh, cv::Mat& image);
    int ObtenerModulo(int i,int j, cv::Mat &image);
    int ObtenerBitC(int lugar);
    int ObtenerBitD(int lugar);
    float Distancia(cv::Point2f P, cv::Point2f Q);					// Obtener distancia entre 2 puntos
    float DistEcuacionRecta(cv::Point2f L, cv::Point2f M, cv::Point2f J);		// Perpendicular Distance of a Point J from line formed by Points L and M; Solution to equation of the line Val = ax+by+c
    float PendienteRecta(cv::Point2f L, cv::Point2f M, int& alignement);	// Slope of a line by two Points L and M on it; Slope of line, S = (x1 -x2) / (y1- y2)
    void ObtenerVertices(std::vector<std::vector<cv::Point> > contours, int c_id,float slope, std::vector<cv::Point2f>& X);
    void ActualizarEsquina(cv::Point2f P, cv::Point2f ref ,float& baseline,  cv::Point2f& corner);
    int ObtenerQR(cv::Mat& image, cv::Mat& qr_raw,int& orientation);
    void ActualizarOrientacionEsquinas(int orientation, std::vector<cv::Point2f> in, std::vector<cv::Point2f> &out);
    bool ObtenerInterseccionPuntos(cv::Point2f a1, cv::Point2f a2, cv::Point2f b1, cv::Point2f b2, cv::Point2f& intersection);
    float cross(cv::Point2f v1,cv::Point2f v2);
    void ConvertirLlave();

    QProcess *proceso;
    QGraphicsScene scene;
    Ui::MainWindow *ui;
    QGraphicsPixmapItem originalPixmap, processedPixmap;
    cv::Mat originalMat, processedMat;
    QImage originalImage, processedImage;
    //ReedMuller* R;
    QBitArray* a_a;//array auxiliar
    QBitArray* a_c;//array codificado
    QBitArray* a_o;//array original
    qint64 llave;
    qint8 *llavebit;
    int *cadena_o;
    int *cadena_r;
    int espaciado;
    int tam_cod;
    int tam_men;
    int wi;
    int hi;
    int tam;//tamaño de los modulos
    int version;//version del simbolo
    int tipo;//tipo de palabra

};

#endif // MAINWINDOW_H
