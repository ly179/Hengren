#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QSharedMemory>
#include "app_process_lock.h"
#include "image_measurement.h"
#include "log_qt_bind.h"
#include "mat_ellipse.h"
#include "opencv2/opencv.hpp"
#if defined(QT_DEBUG) && defined(__SOURCE_CODE_DIR)
#define __WORKDIR __SOURCE_CODE_DIR
#else
#define __WORKDIR ""
#endif
using VideoFrameCall = std::function<void(const QSharedPointer<QImage> &image)>;
int testCount = 0;
int main(int argc, char *argv[])
{
    system("chcp 65001");
    QCoreApplication a(argc, argv);
    if (!uniqueProcess(&a, 122700)) {
        //simpleMessageBox(nullptr, "Error", "The application process has been started up.");
        return 0;
    } else {
        bindQtLog(&a);
        std::string dilateName1 = "c:\\test1\\b05.bmp";
        QImage img1(QString::fromStdString(dilateName1));
        img1 = img1.convertToFormat(QImage::Format_Grayscale8);
        PatternRecognitionAlgorithmSettingParam settingParam;
        settingParam.kind = 2;
        settingParam.algMode = cdsem::PRPOM_MP_M;
        settingParam.algorithm = 4;
        for (int i = 20; i < 40; i++) {
            std::string dilateName = "c:\\test1\\d" + std::to_string(i) + ".bmp";
            if (!QFile::exists(QString::fromStdString(dilateName)))
                continue;
            QImage img(QString::fromStdString(dilateName));
            //ImageOperationWindowHelper::instance()->show();
            qDebug() << dilateName;
            QImage imgGray = img.convertToFormat(QImage::Format_Grayscale8);
            algo::getImageSharpness(imgGray);
            //auto cc = HRMat::detectWaferEdge1(imgGray, i);
            //qDebug() << cc.center << cc.fitLine << cc.contour;
            //HRMat::checkSharpness(imgGray, i);
        }
        //////////////////////////////////////////////////////////////////
        // std::string dilateName1 = "c:\\test1\\b05.bmp";
        // QImage img1(QString::fromStdString(dilateName1));
        // img1 = img1.convertToFormat(QImage::Format_Grayscale8);
        // PatternRecognitionAlgorithmSettingParam settingParam;
        // settingParam.kind = 2;
        // settingParam.algMode = cdsem::PRPOM_MP_M;
        // settingParam.algorithm = 4;
        // for (int i = 15; i < 30; i++) {
        //     std::string dilateName = "c:\\test1\\a" + std::to_string(i) + ".bmp";
        //     if (!QFile::exists(QString::fromStdString(dilateName)))
        //         continue;
        //     QImage img(QString::fromStdString(dilateName));
        //     //ImageOperationWindowHelper::instance()->show();
        //     qDebug() << dilateName;
        //     QImage imgGray = img.convertToFormat(QImage::Format_Grayscale8);
        //     algo::searchFromTemplate(imgGray, img1, settingParam, false);
        //     //HRMat::checkSharpness(imgGray, i);
        // }

        /////////////////////////////////////////////////////////////////////
        // for (int i = 1; i < 10; i++) {
        //     std::string dilateName = "c:\\test1\\c0" + std::to_string(i) + ".bmp";
        //     if (!QFile::exists(QString::fromStdString(dilateName)))
        //         continue;
        //     QImage img(QString::fromStdString(dilateName));
        //     //ImageOperationWindowHelper::instance()->show();
        //     qDebug() << dilateName;
        //     QImage imgGray = img.convertToFormat(QImage::Format_Grayscale8);

        //     HRMat::checkSharpness(imgGray, i);
        // }

        // QList<QPair<QPoint, QPointF>> sll;

        // sll.append(QPair<QPoint, QPointF>(QPoint(0, 0), QPointF(0.119238, 0.0733224)));
        // sll.append(QPair<QPoint, QPointF>(QPoint(-1, 0), QPointF(0.113879, 0.0656065)));
        // sll.append(QPair<QPoint, QPointF>(QPoint(-1, -1), QPointF(0.121927, 0.0600076)));
        // sll.append(QPair<QPoint, QPointF>(QPoint(0, -1), QPointF(0.127284, 0.0677128)));
        // sll.append(QPair<QPoint, QPointF>(QPoint(2, 2), QPointF(0.113868, 0.0998808)));
        // sll.append(QPair<QPoint, QPointF>(QPoint(-3, 2), QPointF(0.087086, 0.0614878)));
        // sll.append(QPair<QPoint, QPointF>(QPoint(-3, -3), QPointF(0.12729, 0.0334476)));
        // sll.append(QPair<QPoint, QPointF>(QPoint(2, -3), QPointF(0.154055, 0.0718571)));
        // auto listTransform1 = algo::calculateTransformMatrixId(sll);
        // QList<double> matrix1 = listTransform1;
        // for (int i = 0; i < sll.length(); i++) {
        //     auto waferPos = sll[i].first;
        //     double x1 = waferPos.x() * matrix1[0] + waferPos.y() * matrix1[2] + matrix1[4];
        //     double y1 = waferPos.x() * matrix1[1] + waferPos.y() * matrix1[3] + matrix1[5];
        //     qInfo()
        //         << waferPos
        //         << QPointF((sll[i].second.x() - x1) * 1000000, (sll[i].second.y() - y1) * 1000000)
        //         << sqrt((sll[i].second.x() - x1) * (sll[i].second.x() - x1)
        //                 + (sll[i].second.y() - y1) * (sll[i].second.y() - y1))
        //                * 1000000;
        // }
        // qInfo() << "==========";

        // QList<QPoint> idl;
        // idl.append(QPoint(0, 0));
        // idl.append(QPoint(-1, 0));
        // idl.append(QPoint(-1, -1));
        // idl.append(QPoint(0, -1));
        // idl.append(QPoint(2, 2));
        // idl.append(QPoint(-3, 2));
        // idl.append(QPoint(-3, -3));
        // idl.append(QPoint(2, -3));

        // QList<QPair<QPointF, QPointF>> matchList;

        // matchList.append(QPair<QPointF, QPointF>(QPointF(0.00151326, 0.00173911),
        //                                          QPointF(0.00155472, 0.00176054)));
        // matchList.append(QPair<QPointF, QPointF>(QPointF(-0.00784674, 0.00173911),
        //                                          QPointF(-0.00783951, 0.00170098)));
        // matchList.append(QPair<QPointF, QPointF>(QPointF(-0.00784674, -0.00806089),
        //                                          QPointF(-0.00778526, -0.0081023)));
        // matchList.append(QPair<QPointF, QPointF>(QPointF(0.00151326, -0.00806089),
        //                                          QPointF(0.00159901, -0.00804734)));
        // matchList.append(
        //     QPair<QPointF, QPointF>(QPointF(0.0202333, 0.0213391), QPointF(0.0201805, 0.0214395)));
        // matchList.append(QPair<QPointF, QPointF>(QPointF(-0.0265667, 0.0213391),
        //                                          QPointF(-0.0266306, 0.0212389)));
        // matchList.append(QPair<QPointF, QPointF>(QPointF(-0.0265667, -0.0276609),
        //                                          QPointF(-0.0264159, -0.0277772)));
        // matchList.append(
        //     QPair<QPointF, QPointF>(QPointF(0.0202333, -0.0276609), QPointF(0.0203987, -0.027553)));
        // auto listTransform = algo::calculateTransformMatrix(matchList);
        // QList<double> matrix = listTransform;
        // for (int i = 0; i < matchList.length(); i++) {
        //     auto waferPos = matchList[i].first;
        //     double x1 = waferPos.x() * matrix[0] + waferPos.y() * matrix[2] + matrix[4];
        //     double y1 = waferPos.x() * matrix[1] + waferPos.y() * matrix[3] + matrix[5];
        //     qInfo() << sqrt((matchList[i].second.x() - x1) * (matchList[i].second.x() - x1)
        //                     + (matchList[i].second.y() - y1) * (matchList[i].second.y() - y1));
        // }

        // QList<QPair<QPointF, double>> wdData1;
        // wdData1.append(QPair<QPointF, double>(QPointF(102.767, 0.0115705), 10.1729));
        // wdData1.append(QPair<QPointF, double>(QPointF(177.155, 0.0341622), 10.1677));
        // wdData1.append(QPair<QPointF, double>(QPointF(221.063, 0.0352658), 10.2096));
        // wdData1.append(QPair<QPointF, double>(QPointF(290.255, 0.0145124), 10.2196));
        // wdData1.append(QPair<QPointF, double>(QPointF(353.764, 0.0368107), 10.2146));
        // wdData1.append(QPair<QPointF, double>(QPointF(35.7282, 0.0357573), 10.1451));
        // wdData1.append(QPair<QPointF, double>(QPointF(64.8814, 0.0505402), 10.0819));
        // wdData1.append(QPair<QPointF, double>(QPointF(105.637, 0.0375451), 10.0835));
        // wdData1.append(QPair<QPointF, double>(QPointF(147.511, 0.0494277), 10.0878));
        // wdData1.append(QPair<QPointF, double>(QPointF(166.984, 0.0751888), 10.0878));
        // wdData1.append(QPair<QPointF, double>(QPointF(186.898, 0.0661705), 10.1585));
        // wdData1.append(QPair<QPointF, double>(QPointF(209.454, 0.0667487), 10.218));
        // wdData1.append(QPair<QPointF, double>(QPointF(228.768, 0.0767057), 10.2767));
        // wdData1.append(QPair<QPointF, double>(QPointF(248.44, 0.0516992), 10.2803));
        // wdData1.append(QPair<QPointF, double>(QPointF(288.104, 0.0404956), 10.2823));
        // wdData1.append(QPair<QPointF, double>(QPointF(326.821, 0.05277), 10.2809));
        // wdData1.append(QPair<QPointF, double>(QPointF(4.70148, 0.0683905), 10.2001));
        // wdData1.append(QPair<QPointF, double>(QPointF(26.6969, 0.0678333), 10.1393));
        // wdData1.append(QPair<QPointF, double>(QPointF(46.2252, 0.0766591), 10.0683));
        // wdData1.append(QPair<QPointF, double>(QPointF(78.8704, 0.0719779), 10.0048));
        // wdData1.append(QPair<QPointF, double>(QPointF(106.161, 0.0635407), 10.009));
        // wdData1.append(QPair<QPointF, double>(QPointF(133.77, 0.0712149), 10.0372));
        // wdData1.append(QPair<QPointF, double>(QPointF(152.645, 0.0909959), 10.0192));
        // wdData1.append(QPair<QPointF, double>(QPointF(126.676, 0.0951327), 9.96022));
        // wdData1.append(QPair<QPointF, double>(QPointF(106.38, 0.089535), 9.93969));
        // wdData1.append(QPair<QPointF, double>(QPointF(287.641, 0.0664877), 10.3348));
        // wdData1.append(QPair<QPointF, double>(QPointF(261.098, 0.0738444), 10.3353));
        // QList<double> wdTrance = algo::calWDMap(wdData1);
        // qInfo() << "wdTrance: " << wdTrance;
        // double maxV = 0;
        // double minV = 0;
        // for (int i = 0; i < wdData1.length(); i++) {
        //     QPair<QPointF, double> p = wdData1[i];
        //     double nwd = algo::calWDValue(wdTrance, p.first.x(), p.first.y());
        //     qInfo() << "p: " << p << ",  nwd: " << nwd << ",  dif: " << nwd - p.second;
        //     if (nwd - p.second > maxV)
        //         maxV = nwd - p.second;
        //     if (nwd - p.second < minV)
        //         minV = nwd - p.second;
        // }
        // qInfo() << "maxV: " << maxV << ", minV: " << minV;
        // QList<QPair<QPointF, double>> wdData2;
        // wdData2.append(QPair<QPointF, double>(QPointF(0, 0.060), 5.286));
        // wdData2.append(QPair<QPointF, double>(QPointF(30, 0.060), 5.4));
        // wdData2.append(QPair<QPointF, double>(QPointF(60, 0.060), 5.451));
        // wdData2.append(QPair<QPointF, double>(QPointF(90, 0.060), 5.427));
        // wdData2.append(QPair<QPointF, double>(QPointF(120, 0.060), 5.196));
        // wdData2.append(QPair<QPointF, double>(QPointF(180, 0.060), 5.031));
        // wdData2.append(QPair<QPointF, double>(QPointF(210, 0.060), 4.913));
        // wdData2.append(QPair<QPointF, double>(QPointF(240, 0.060), 4.844));
        // wdData2.append(QPair<QPointF, double>(QPointF(270, 0.060), 4.875));
        // wdData2.append(QPair<QPointF, double>(QPointF(300, 0.060), 4.968));
        // wdData2.append(QPair<QPointF, double>(QPointF(330, 0.060), 5.131));
        // // wdData.append(QPair<QPointF, double>(QPointF(330, 0), 5.207));
        // // wdData.append(QPair<QPointF, double>(QPointF(90, 0), 5.179));
        // wdData2.append(QPair<QPointF, double>(QPointF(120, 0.00269), 5.204));
        // wdData2.append(QPair<QPointF, double>(QPointF(260, 0.00269), 5.16));
        // wdData2.append(QPair<QPointF, double>(QPointF(171.28, 0.0362), 5.092));
        // wdData2.append(QPair<QPointF, double>(QPointF(256.69, 0.0138), 5.094));
        // wdData2.append(QPair<QPointF, double>(QPointF(81.74, 0.0192), 5.259));

        // wdTrance = algo::calWDMap(wdData2);
        // qInfo() << "wdTrance: " << wdTrance;
        // maxV = 0;
        // minV = 0;
        // for (int i = 0; i < wdData2.length(); i++) {
        //     QPair<QPointF, double> p = wdData2[i];
        //     double nwd = algo::calWDValue(wdTrance, p.first.x(), p.first.y());
        //     qInfo() << "p: " << p << ",  nwd: " << nwd << ",  dif: " << nwd - p.second;
        //     if (nwd - p.second > maxV)
        //         maxV = nwd - p.second;
        //     if (nwd - p.second < minV)
        //         minV = nwd - p.second;
        // }
        // qInfo() << "maxV: " << maxV << ", minV: " << minV;

        // //double algo::calWDValue(QList<double> wdTrance, double rotation, double radius)
        // //QList<float> MEASUREMENT_ALGO_EXPORT calWDMap(QList<QPair<QPoint, double>> wdData);

        // QList<QPair<QPointF, QPointF>> wdData;

        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.240281141, 1.96459), QPointF(0, 0)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.240281141, 2.16301), QPointF(-2.37745, 1.77459)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.240281141, 2.36322), QPointF(-4.88878, 3.06271)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.240281141, 2.564), QPointF(-7.2737, 4.03555)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.240281141, 2.76681), QPointF(-10.0484, 5.20578)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.240281141, 2.96915), QPointF(-12.2332, 6.45415)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.240281141, 3.17222), QPointF(-15.0773, 7.88862)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.240281141, 3.37536), QPointF(-17.4901, 9.34116)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.240281141, 3.57925), QPointF(-20.2004, 11.2621)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.240281141, 3.78192), QPointF(-22.4947, 13.4676)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.240281141, 3.98538), QPointF(-25.0141, 15.8239)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.240281141, 4.18901), QPointF(-26.9112, 18.6338)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.316227766, 1.9691), QPointF(-5.58263, 2.01644)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.316227766, 2.16927), QPointF(-9.06015, 4.11897)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.316227766, 2.37181), QPointF(-11.7766, 6.1465)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.316227766, 2.57531), QPointF(-15.3589, 8.19121)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.316227766, 2.78166), QPointF(-19.0552, 10.338)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.316227766, 2.98939), QPointF(-22.3189, 13.4112)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.316227766, 3.19697), QPointF(-25.7554, 17.5991)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 1.96254), QPointF(3.69674, -1.43981)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 2.16012), QPointF(1.33788, 0.0149861)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 2.36087), QPointF(-0.609483, 0.899293)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 2.56138), QPointF(-2.55684, 1.7836)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 2.76329), QPointF(-4.93247, 2.59655)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 2.9645), QPointF(-6.91827, 3.2121)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 3.16675), QPointF(-9.29389, 4.02505)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 3.3681), QPointF(-11.0919, 5.12932)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 3.57065), QPointF(-13.2165, 6.27335)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 3.77223), QPointF(-15.2903, 7.47294)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 3.97453), QPointF(-17.079, 8.41732)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 4.17605), QPointF(-19.2544, 9.50579)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 4.37841), QPointF(-20.9569, 10.8278)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 4.58076), QPointF(-22.7952, 12.6249)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 4.78542), QPointF(-24.903, 14.4108)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 4.98784), QPointF(-26.5411, 16.4835)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 5.1908), QPointF(-28.096, 18.9871)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 1.96202), QPointF(5.83548, -2.04069)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 2.16014), QPointF(3.93585, -1.04753)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 2.36035), QPointF(2.10872, -0.371903)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 2.56056), QPointF(0.129112, 0.137054)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 2.76246), QPointF(-1.47305, 0.661815)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 2.96252), QPointF(-3.45575, 1.22407)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 3.16447), QPointF(-5.39072, 1.89518)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 3.36473), QPointF(-7.17323, 2.73296)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 3.5669), QPointF(-9.1082, 3.40407)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 3.76724), QPointF(-10.5102, 4.20436)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 3.96946), QPointF(-12.0708, 4.94458)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 4.16972), QPointF(-13.7852, 5.46877)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 4.37212), QPointF(-15.4661, 6.41766)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 4.57269), QPointF(-17.0391, 7.37107)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 4.77508), QPointF(-18.9833, 8.20208)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 4.97575), QPointF(-20.4392, 9.00011)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 5.17835), QPointF(-21.7935, 9.90925)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 5.37851), QPointF(-23.4465, 11.2374)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 5.58188), QPointF(-25.0368, 13.419)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 5.78279), QPointF(-26.2373, 15.4031)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 5.98567), QPointF(-27.4687, 17.9203)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 6.18617), QPointF(-28.6865, 21.1326)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 1.96276), QPointF(7.22243, -2.72649)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 2.16048), QPointF(5.43374, -1.78211)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 2.36102), QPointF(4.00263, -1.41047)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 2.56082), QPointF(2.46058, -0.99005)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 2.76162), QPointF(0.652027, -0.634217)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 2.9624), QPointF(-0.77289, -0.369177)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 3.16351), QPointF(-2.42588, 0.100027)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 3.36437), QPointF(-4.24682, 0.669056)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 3.56555), QPointF(-5.69651, 1.36049)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 3.76608), QPointF(-7.19392, 1.94306)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 3.96718), QPointF(-8.8531, 2.51886)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 4.16765), QPointF(-10.1795, 2.94832)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 4.37024), QPointF(-11.3441, 3.38454)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 4.56999), QPointF(-12.8384, 3.91382)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 4.77243), QPointF(-14.4529, 4.65177)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 4.97208), QPointF(-15.7254, 5.08348)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 5.17464), QPointF(-16.9452, 5.4681)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 5.37398), QPointF(-18.5275, 6.58141)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 5.57468), QPointF(-20.0683, 7.91017)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 5.77526), QPointF(-21.0575, 9.04831)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 5.97744), QPointF(-22.0635, 10.4036)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 6.17895), QPointF(-23.3625, 12.2233)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 6.3804), QPointF(-24.5258, 13.5679)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 6.5804), QPointF(-25.6352, 14.9147)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 6.78409), QPointF(-26.8711, 16.5768)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 6.98601), QPointF(-28.1608, 18.2367)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 7.18863), QPointF(-29.3414, 20.8094)));

        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.177827941, 2.15958), QPointF(6.291777, -2.103436)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 2.16036), QPointF(5.43374, -1.78211)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.188030155, 2.16115), QPointF(4.441617, -1.474325)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.194413084, 2.16177), QPointF(3.508587, -1.093446)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.20205155, 2.16243), QPointF(2.299427, -0.673316)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.211474253, 2.16285), QPointF(0.806187, -0.147606)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.217118521, 2.16386), QPointF(-0.028003, -0.025266)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.223606798, 2.16456), QPointF(-0.601963, 0.190484)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.231197423, 2.16486), QPointF(-1.586133, 0.431944)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.240281141, 2.16569), QPointF(-2.653253, 0.799284)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.251486686, 2.16681), QPointF(-4.008123, 1.215084)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.265914795, 2.16832), QPointF(-5.568423, 1.734024)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.285744043, 2.1698), QPointF(-7.067423, 2.264324)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.316227766, 2.17007), QPointF(-9.503823, 3.298964)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.169904424, 2.16233), QPointF(7.110923, -2.753957)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.163481266, 2.16206), QPointF(7.831217, -3.145936)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.158113883, 2.16301), QPointF(8.401527, -3.418806)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.153525978, 2.16444), QPointF(8.904797, -3.698446)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.149534878, 2.16603), QPointF(9.395057, -3.847736)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.141421356, 2.17075), QPointF(9.930127, -4.392676)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 2.17793), QPointF(10.425437, -4.605996)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.130011865, 2.1818), QPointF(10.856607, -4.828376)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.125743343, 2.18718), QPointF(11.212787, -4.991206)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.122094717, 2.19201), QPointF(11.426917, -5.101246)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 2.19647), QPointF(11.700147, -5.138186)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.113621937, 2.20453), QPointF(12.064267, -5.367346)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.109326511, 2.21147), QPointF(12.302257, -5.676366)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.105737126, 2.2167), QPointF(12.754377, -6.095426)));

        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 2.21451), QPointF(11.700147, -5.138186)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 2.71586), QPointF(10.424577, -5.853585)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 3.21837), QPointF(9.468077, -6.966396)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 3.71938), QPointF(7.905257, -7.549696)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 4.22129), QPointF(6.079757, -7.300246)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 5.22309), QPointF(3.399697, -7.620436)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 6.22088), QPointF(1.499547, -8.560016)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 7.21822), QPointF(0.070347, -10.257606)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 8.21789), QPointF(-1.991553, -10.300546)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 9.21782), QPointF(-4.482553, -10.386826)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 10.2145), QPointF(-5.452553, -10.480626)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 11.212), QPointF(-6.349053, -11.641086)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 12.2108), QPointF(-8.168253, -11.444706)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 13.211), QPointF(-10.214653, -11.163866)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 14.2093),
        //                                       QPointF(-11.109253, -10.981526)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 15.2077),
        //                                       QPointF(-11.478553, -12.464666)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 16.2079),
        //                                       QPointF(-13.004053, -12.453456)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 17.2088),
        //                                       QPointF(-14.744053, -12.463906)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 18.2067),
        //                                       QPointF(-15.024553, -11.953106)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 19.2077),
        //                                       QPointF(-15.099353, -14.534296)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 2.20988), QPointF(12.320054, -4.910322)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 2.71146), QPointF(10.479937, -5.897546)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 3.21297), QPointF(9.201617, -7.042856)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 3.71483), QPointF(7.780617, -7.450716)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 4.21628), QPointF(5.806937, -7.323646)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 5.21818), QPointF(3.139597, -7.749956)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 6.21616), QPointF(0.904047, -7.703026)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 7.2136), QPointF(-0.650653, -10.165146)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 8.21231), QPointF(-2.819753, -10.218916)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 9.2128), QPointF(-5.080753, -9.959746)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 10.2093), QPointF(-6.266253, -9.162336)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 11.2074), QPointF(-7.575453, -10.955216)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 12.2057), QPointF(-9.214653, -10.901766)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 13.2056), QPointF(-11.160153, -10.557036)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 14.2046), QPointF(-12.332853, -9.865746)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 15.2051), QPointF(-13.414853, -11.743086)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 16.2047), QPointF(-14.543153, -11.423236)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 17.21), QPointF(-15.592253, -11.310206)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 18.2049), QPointF(-16.001453, -10.649216)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.12077458, 19.2068), QPointF(-16.161653, -12.970516)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.126541754, 2.20056), QPointF(11.985952, -4.404944)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.126541754, 2.70133), QPointF(9.935337, -5.465051)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.126541754, 3.20298), QPointF(8.637937, -6.451176)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.126541754, 3.70439), QPointF(6.686937, -6.966256)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.126541754, 4.20551), QPointF(4.848717, -6.610696)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.126541754, 5.20675), QPointF(1.559547, -6.831266)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.126541754, 6.20391), QPointF(-0.628653, -6.725866)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.126541754, 7.2015), QPointF(-2.723353, -8.759176)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.126541754, 8.2001), QPointF(-5.050653, -8.399286)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.126541754, 9.20006), QPointF(-7.390653, -7.933276)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.126541754, 10.1981), QPointF(-9.377953, -6.787206)));

        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 2.20669), QPointF(11.700147, -5.138186)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 2.71074), QPointF(9.951827, -6.438366)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 3.21522), QPointF(8.506247, -7.546856)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 3.71738), QPointF(7.160717, -7.678576)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 4.2184), QPointF(5.944287, -7.528746)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 5.22002), QPointF(3.441467, -8.421776)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 6.218), QPointF(1.341447, -8.146346)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 7.21653), QPointF(-0.576053, -10.752506)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 8.21579), QPointF(-2.253353, -10.380686)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 9.21546), QPointF(-4.579853, -10.933686)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 10.2105), QPointF(-5.484353, -10.215316)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 11.2091), QPointF(-7.055453, -12.088346)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 12.2082), QPointF(-8.335453, -11.407886)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 13.2109),
        //                                       QPointF(-10.283753, -11.493066)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 14.205), QPointF(-11.238353, -11.263086)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 15.2074),
        //                                       QPointF(-12.708553, -13.072216)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 16.2054),
        //                                       QPointF(-13.332153, -12.432886)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 17.2089),
        //                                       QPointF(-14.860453, -12.851586)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 18.2064),
        //                                       QPointF(-15.136853, -12.392006)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 19.2051),
        //                                       QPointF(-15.719853, -14.809696)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 20.207), QPointF(-15.577553, -14.202516)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 21.2074),
        //                                       QPointF(-16.616753, -14.625516)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 22.2065),
        //                                       QPointF(-16.414553, -14.065976)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 23.2085),
        //                                       QPointF(-16.735653, -16.403486)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 24.2148),
        //                                       QPointF(-17.205253, -16.126686)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.118920712, 25.219), QPointF(-17.188453, -16.283986)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 26.2168),
        //                                       QPointF(-16.812553, -15.814286)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 27.2318),
        //                                       QPointF(-16.644653, -18.156186)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 28.2111),
        //                                       QPointF(-16.606053, -18.042786)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 29.3743),
        //                                       QPointF(-16.352953, -17.907786)));
        // wdData.append(QPair<QPointF, QPointF>(QPointF(0.118920712, 30.3743),
        //                                       QPointF(-15.799353, -17.579186)));

        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 2.1857), QPointF(11.6876, -3.96396)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 2.68697), QPointF(9.88204, -4.786582)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 3.18864), QPointF(8.20919, -5.81062)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 3.6903), QPointF(5.90733, -5.66298)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 4.19133), QPointF(3.79733, -4.851523)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 5.19195), QPointF(-0.1645, -4.441878)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 6.18939), QPointF(-2.7218, -4.427445)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 7.18732), QPointF(-4.7665, -5.97237)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 8.1875), QPointF(-8.1374, -4.639639)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 9.18847), QPointF(-11.251, -3.607301)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 10.1873), QPointF(-13.7556, -2.67457)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 11.1866), QPointF(-15.3466, -3.475528)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 12.1889), QPointF(-18.1992, -1.45017)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 13.1942), QPointF(-20.4292, -0.54342)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 14.1958), QPointF(-21.7584, 0.51216)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 15.2), QPointF(-22.8576, 0.13679)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 16.2064), QPointF(-24.5004, 2.4496)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 17.2118), QPointF(-25.6867, 4.15989)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 18.2183), QPointF(-26.1821, 5.99365)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 19.2227), QPointF(-26.5981, 6.27594)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.135120015, 20.2273), QPointF(-26.5271, 9.72434)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.144400282, 2.18356), QPointF(10.10209, -3.9050977)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.144400282, 2.68389), QPointF(7.73473, -4.569635)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.144400282, 3.18457), QPointF(5.481, -5.2764)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.144400282, 3.68463), QPointF(3.93004, -5.05294)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.144400282, 4.18441), QPointF(1.4319, -4.173269)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.144400282, 5.18484), QPointF(-2.8836, -3.530812)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.144400282, 6.18277), QPointF(-5.9944, -2.06856)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.144400282, 7.18202), QPointF(-9.8163, -2.82628)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.144400282, 8.18181), QPointF(-13.3944, -1.08899)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.144400282, 9.18516), QPointF(-17.1425, 0.2552)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.144400282, 10.186), QPointF(-20.1815, 2.47657)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.144400282, 11.1909), QPointF(-23.0452, 2.78229)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.144400282, 12.1964), QPointF(-24.8732, 5.77038)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.144400282, 13.2045), QPointF(-27.6159, 8.23644)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.144400282, 14.2115), QPointF(-29.3875, 11.65994)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.144400282, 15.217), QPointF(-30.862, 14.41514)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.15146476, 2.17535), QPointF(9.88162, -3.441948)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.15146476, 2.67677), QPointF(6.49922, -4.260613)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.15146476, 3.17793), QPointF(4.45007, -4.409681)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.15146476, 3.67794), QPointF(2.09733, -3.83755)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.15146476, 4.1785), QPointF(-0.4337, -3.17186)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.15146476, 5.17975), QPointF(-5.1472, -1.92516)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.15146476, 6.17823), QPointF(-9.4953, -0.10451)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.15146476, 7.1783), QPointF(-13.9517, -0.55037)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.15146476, 8.18009), QPointF(-17.7707, 2.29038)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.15146476, 9.18674), QPointF(-22.0624, 4.54636)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.15146476, 10.1903), QPointF(-24.9604, 7.85604)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.15146476, 11.1966), QPointF(-27.9794, 9.00534)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.15146476, 12.204), QPointF(-30.2674, 14.47104)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.160685684, 2.17149), QPointF(8.29885, -2.95318)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.160685684, 2.67182), QPointF(4.88466, -3.506532)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.160685684, 3.17279), QPointF(1.79099, -3.546261)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.160685684, 3.67315), QPointF(-0.7608, -2.67201)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.160685684, 4.17321), QPointF(-3.4772, -1.33103)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.160685684, 5.17522), QPointF(-9.0662, -0.06118)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.160685684, 6.17426), QPointF(-14.3738, 2.95355)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.160685684, 7.17704), QPointF(-19.4056, 3.68478)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.160685684, 8.18173), QPointF(-23.8345, 7.53804)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.160685684, 9.18939), QPointF(-28.1434, 11.29594)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.160685684, 10.1942), QPointF(-31.3448, 18.49544)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.17364082, 2.16789), QPointF(6.09198, -2.25683)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.17364082, 2.66731), QPointF(2.3956, -2.25001)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.17364082, 3.16838), QPointF(-1.099, -2.11541)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.17364082, 3.66928), QPointF(-4.2789, -0.98236)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.17364082, 4.16987), QPointF(-7.188, 0.60767)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.17364082, 5.17416), QPointF(-14.761, 3.50104)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.17364082, 6.17628), QPointF(-20.6462, 7.69474)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.17364082, 7.18135), QPointF(-26.2479, 10.46284)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.17364082, 8.18999), QPointF(-31.1583, 19.69154)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 2.16686), QPointF(5.43374, -1.78211)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 2.66664), QPointF(0.7314, -1.55257)));
        // wdData.append(
        //     QPair<QPointF, QPointF>(QPointF(0.182574186, 3.16733), QPointF(-3.0631, -1.17974)));

        // std::string dilateName2 = "c:\\test1\\a04.bmp";

        // QImage img2(QString::fromStdString(dilateName2));
        // img2 = img2.convertToFormat(QImage::Format_Grayscale8);

        // // for (int i = 11; i < 55; i += 2)
        // //     QPointF p = algo::FindHarrisCorner(img1, i);
        // //qInfo() << "p: " << p;
        // MatchPatternResult res = algo::searchFromTemplate(img2, img1, settingParam, false);
        //qInfo() << "score: " << res.score << " position: " << res.shift;

        // QList<float> waferTransformMatrix
        //     = {0.999094, 0.00176677, -0.00454078, 0.999368, -1.67954e-05, -2.02047e-05};
        // algo::inverseTransformation(waferTransformMatrix);

        // for (int i = 1; i < 10; i++) {
        //     std::string dilateName = "c:\\test1\\c0" + std::to_string(i) + ".bmp";
        //     if (!QFile::exists(QString::fromStdString(dilateName)))
        //         continue;
        //     QImage img(QString::fromStdString(dilateName));
        //     //ImageOperationWindowHelper::instance()->show();
        //     qDebug() << dilateName;
        //     QImage imgGray = img.convertToFormat(QImage::Format_Grayscale8);

        //     HRMat::checkSharpness(imgGray, i);
        // }
        // QList<QPointF> listWaferEdge1;
        // listWaferEdge1.append(QPointF(20.537, 52.6863));
        // listWaferEdge1.append(QPointF(25.488, 36.0709));
        // listWaferEdge1.append(QPointF(33.1893, 20.6522));
        // listWaferEdge1.append(QPointF(43.3605, 6.85946));
        // listWaferEdge1.append(QPointF(55.6577, -4.94981));
        // //listWaferEdge1.append(QPointF(84.5883, -22.9005));
        // listWaferEdge1.append(QPointF(135.008, -25.8237));
        // listWaferEdge1.append(QPointF(151.33, -21.669));
        // listWaferEdge1.append(QPointF(166.747, -14.8199));
        // listWaferEdge1.append(QPointF(203.382, 19.8604));
        // listWaferEdge1.append(QPointF(216.311, 51.5738));
        // listWaferEdge1.append(QPointF(213.884, 103.317));
        // listWaferEdge1.append(QPointF(207.053, 119.595));
        // listWaferEdge1.append(QPointF(154.732, 166.188));
        // //listWaferEdge1.append(QPointF(137.389, 171.024));
        // listWaferEdge1.append(QPointF(67.7379, 159.111));
        // listWaferEdge1.append(QPointF(40.6817, 135.755));
        // listWaferEdge1.append(QPointF(19.5521, 87.4892));
        // HRMat::ContourCircle waferCircle;
        // waferCircle = HRMat::fitCircle(listWaferEdge1);
        // qInfo() << "WaferEdg1 finish radius=" << waferCircle.radius << ", center=("
        //         << waferCircle.center.x() << ", " << waferCircle.center.y() << ")";
        // for (int i = 0; i < listWaferEdge1.length(); i++) {
        //     QPointF basePos = listWaferEdge1[i];
        //     double dis = sqrt(
        //         (waferCircle.center.x() - basePos.x()) * (waferCircle.center.x() - basePos.x())
        //         + (waferCircle.center.y() - basePos.y()) * (waferCircle.center.y() - basePos.y()));

        //     qInfo() << "listWaferEdge (" << listWaferEdge1[i].x() << ", " << listWaferEdge1[i].y()
        //             << ")  dis=" << dis << ",   i=" << i << "°";
        // }
        // listWaferEdge ( 20.537 ,  52.6863 )  dis= 100.146 ,   rotation= 10 °
        // listWaferEdge ( 25.488 ,  36.0709 )  dis= 100.092 ,   rotation= 20 °
        // listWaferEdge ( 33.1893 ,  20.6522 )  dis= 100.026 ,   rotation= 30 °
        // listWaferEdge ( 43.3605 ,  6.85946 )  dis= 99.9581 ,   rotation= 40 °
        // listWaferEdge ( 55.6577 ,  -4.94981 )  dis= 99.907 ,   rotation= 50 °
        // listWaferEdge ( 84.5883 ,  -22.9005 )  dis= 101.372 ,   rotation= 70 °
        // listWaferEdge ( 135.008 ,  -25.8237 )  dis= 99.7308 ,   rotation= 100 °
        // listWaferEdge ( 151.33 ,  -21.669 )  dis= 99.7247 ,   rotation= 110 °
        // listWaferEdge ( 166.747 ,  -14.8199 )  dis= 99.726 ,   rotation= 120 °
        //-------- listWaferEdge ( 181.775 ,  -6.59457 )  dis= 101.221 ,   rotation= 130 °
        //--------- listWaferEdge ( 194.292 ,  5.25205 )  dis= 101.224 ,   rotation= 140 °
        // listWaferEdge ( 203.382 ,  19.8604 )  dis= 99.7506 ,   rotation= 150 °
        // listWaferEdge ( 216.311 ,  51.5738 )  dis= 99.8514 ,   rotation= 170 °
        // listWaferEdge ( 213.884 ,  103.317 )  dis= 100.038 ,   rotation= 200 °
        // listWaferEdge ( 207.053 ,  119.595 )  dis= 100.1 ,   rotation= 210 °
        // listWaferEdge ( 154.732 ,  166.188 )  dis= 100.322 ,   rotation= 250 °
        // listWaferEdge ( 137.389 ,  171.024 )  dis= 100.22 ,   rotation= 260 °
        // listWaferEdge ( 67.7379 ,  159.111 )  dis= 100.433 ,   rotation= 300 °
        // listWaferEdge ( 40.6817 ,  135.755 )  dis= 100.392 ,   rotation= 320 °
        // listWaferEdge ( 19.5521 ,  87.4892 )  dis= 100.256 ,   rotation= 350 °

        //HRMat::detectEllipse(imgGray, size);

        // SingleProcedureHelper::get()->openProcedure(
        //     [](bool b) { qDebug() << "precedure open status: " << b; });
        // SingleProcedureHelper::get()->runTask(
        //     [](TSC, OperationResult& r) -> PSR_Result {
        //         PSR_Result result = SingleProcedureHelper::get()->createResult(PSC_WAIT_NOTIFY,
        //                                                                        5000);
        //         result->timer()->startTask(ProcedureTimerTask()
        //                                        .setInterval(1000)
        //                                        .setLoop(true)
        //                                        .setTask([](OperationResult&) {
        //                                            //TODO check stage is over.
        //                                            qDebug() << "loop timer....";
        //                                            if (++testCount > 5) {
        //                                                return true;
        //                                            }
        //                                            return false;
        //                                        })
        //                                        .setFinishedNotify([result](const OperationResult&) {
        //                                            result->notiyFinishedOnce();
        //                                        }));
        //         return result;
        //     },
        //     [](const OperationResult& result, const StepTaskCallbackStatusParam& param) -> bool {
        //         qDebug() << "next step: result: " << result.errCode << ", msg: " << result.errMsg
        //                  << ", task staus: " << param.taskStatus
        //                  << ", p status: " << param.procedureStatus;
        //         return false;
        //     });
        // QObject::connect(SingleProcedureHelper::get(), &ProcedureHelper::procedureStarted, &a, []() {
        //     qDebug() << "procedure started!";
        // });
        // QObject::connect(SingleProcedureHelper::get(),
        //                  &ProcedureHelper::procedureFinished,
        //                  &a,
        //                  []() { qDebug() << "procedure finished!"; });

        // auto res = algo::searchFromTemplate(imgGray, tempGray);
        // qInfo() << "score: " << res.score << " position: " << res.shift;
        // QRect patternRect(res.shift.x() - tempGray.width() / 2 + imgGray.width() / 2,
        //                   res.shift.y() - tempGray.height() / 2 + imgGray.height() / 2,
        //                   tempGray.width(),
        //                   tempGray.height());
        // QList<QPair<float, QRect>> similarRect;
        // int score = algo::getPatternQuality(imgGray, patternRect, similarRect);
        // qInfo() << "Quality score: " << score;
        //emit ImageOperationWindowHelper::get() -> liveImageReady(imgGray);
        //img.toImageFormat(QImage::Format_Grayscale8);

        // ImageOperationWindowHelper::instance().
        // logView

        return a.exec();
    }
}
