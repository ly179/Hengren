#include <QApplication>
#include "main_prepared.h"
#include "arithmetic_formula_handler.h"

#include "jkqtplotter/jkqtplotter.h"
#include "jkqtplotter/graphs/jkqtplines.h"

#include "hasp_api_cpp.h"
unsigned char vendorCode[] =
"j98wkom0Gv2uGm3d6IQsjhwRpLkg77DaVlQs1xkz0cl53pRqB5rVKdC6FhZtVtydDoewi9bya6vCsa2X"
"CXbXh7t9JTX52R1cjr3GxrynOeSnWN/8zpGTp3la6pqB/yI6NgLVvec2MVRa6Pxz4hig2M3eOdsGaEVI"
"ncPSoHclXPWeQMiR1sCB42u3vSQJCbck8/rn5SPS4/mo8QdqHL12JmG7UPOTUMcF9e8qGGic2webEI9u"
"HQtteEVEwYNgLfAMlPOuRZwUQWh0rar9CEg1DbgZ57ZXSC1vm650zk4sM4DTrdBf0tjIU8b+t5Rj8zaq"
"ndg2m8InOvXQ+15RnLpo4pjLklA7MOm1Zzl1dihclm60Ld/Vcb2jxxNJ+8hJj8BAh8+Kg2bXYoLXdhAi"
"dYM2SHqG0rz79nSx5RuxrAvDK86wFcTReSkNLmxCjd+YgeuGabgXmUyJIO5/RxQxh36HXFHO4hCrx3fS"
"ZX2M6j09FGF2c2Caj6O2SqHB7yugmdto37T2Ti+NrWd/nFOzKq/I5wWGgVII1bIFZNskm+C7JkyP5AuK"
"JePQ6Gk8o8TqW8qRjvkjknspOMScZwQNkGDEtnB13Zor3q3E4T8kpYkTO5xQETVq3lVOwvp0WPtVbgbX"
"prNgN3pZrI1JIQPYUUs6lFzguDq/UjLl7bVPBNVvLov3CMAARlpvYvS+xPMR4Kxx1zw0rjCp9++DHSwM"
"FedmDufYlp9PmbnA2SgpL/ORVA/+87m99d/WRlOdtxUnwjlj3o00ziZZwWPtT51caDAsQMHb4cc+f1Dt"
"OCZ3pX9B6SWYTDiKt5M59qWYmXerqO9RPv9PuO0wVCYuAs2+vJvNP4DpOyhq6TMRtruu/Lgy5JcWwGHF"
"/dxIiZ6HWJ7B5hXNANVffE/eh/CXDNvkj9lxox/08Xx76p4TG2AxPHFni46rn3hvV7reJxWKOTwtpNOL"
"bdd1h/9AOCkjuKvCkPZ4yw==";




int main(int argc, char* argv[]) {
    // system("chcp 65001");
    QApplication a(argc, argv);

    ChaspFeature feature = ChaspFeature::defaultFeature();
    Chasp hasp(feature);
    haspStatus status = hasp.login(vendorCode);
    if (!HASP_SUCCEEDED(status)) {
        qDebug() << "Failed to login dog";
    } else {
        qDebug() << "dog log success.";
        ChaspFile file = hasp.getFile();
        qDebug() << file.getFileId();
        hasp_size_t size;
        status = file.getFileSize(size);
        qDebug() << "size status: " << status << ", size: " << size;
    }

    status = hasp.logout();
    if (!HASP_SUCCEEDED(status)) {
        qDebug() << "Failed to logout dog";
    } else {
        qDebug() << "dog logout success.";
    }

    // QString text = "5 >= 4 and 3 == 2 ? (1 == 2 or 3 == 4 ? 10 : 20 *12) : (3 ==9 and (4 == 4 and 5 == 5) ? pow(2, 4) : pow(2, 3))";
    // #// QString text = "5   ==3   ?2   :1";
    // QString text = "5  == 4  or 3  ==2  and 4  ==4 ？ 10：20";
    // QString text = "1.2 + 3 * 4 ^2 ^ 3";
    // QString text = "($3 + $2) + 1";
    // QString text = "!(1==2)";
    // QString text = "true and (1==2 ? 2 : 3)";
    // QString text = "1.2 > 4 and 5";
    // QString text = "pi()";
    // QString text = "（$3 + $2） × 5";
    // QString text = "true & false";
    // QString text = "sqrt(8, 3)";
    // QString text = "log(5, 125)";
    // QString text = "!pi()";
    // QString text = "xx a ";
    //函数解析尚未实现，会崩溃
    // QString text = "pow";
    //false   or3  ==2  and4  ==4 ？ 10：20
    //false  orfalse  and4  ==4 ？ 10：20
    //
    // ArithmeticFormulaHandler h;
    // h.setVariable("$1", ArithmeticFormulaVariable {true, AFVT_BOOL});
    // h.setVariable("$2", ArithmeticFormulaVariable {FloatNumber<10>(12), AFVT_NUMBER});
    // h.setVariable("$3", ArithmeticFormulaVariable {FloatNumber<10>(9), AFVT_NUMBER});
    //
    // {
    //     ArithmeticFormulaFunction func;
    //     func.types.reserve(2);
    //     func.types.push_back(AFVT_NUMBER);
    //     func.types.push_back(AFVT_NUMBER);
    //     func.returnType = AFVT_NUMBER;
    //     func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult&){
    //         return floatNumberPow(std::get<ArithmeticNumber>(params[0]), std::get<ArithmeticNumber>(params[1]));
    //     };
    //     h.registFunction("pow", func);
    // }
    // {
    //     ArithmeticFormulaFunction func;
    //     func.returnType = AFVT_NUMBER;
    //     func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult&){
    //         return ArithmeticNumber("3.141592653");
    //     };
    //     h.registFunction("pi", func);
    // }
    // {
    //     ArithmeticFormulaFunction func;
    //     func.types.reserve(2);
    //     func.types.push_back(AFVT_NUMBER);
    //     func.types.push_back(AFVT_NUMBER);
    //     func.returnType = AFVT_NUMBER;
    //     func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult&){
    //         return floatNumberPow(std::get<ArithmeticNumber>(params[0]), ArithmeticNumber(1) / std::get<ArithmeticNumber>(params[1]));
    //     };
    //     registGlobalArithmeticFormualFunction("sqrt", func);
    // }
    // {
    //     ArithmeticFormulaFunction func;
    //     func.types.reserve(2);
    //     func.types.push_back(AFVT_NUMBER);
    //     func.types.push_back(AFVT_NUMBER);
    //     func.returnType = AFVT_NUMBER;
    //     func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
    //         const ArithmeticNumber& b = std::get<ArithmeticNumber>(params[0]);
    //         const ArithmeticNumber& x = std::get<ArithmeticNumber>(params[1]);
    //         if (x <= 0 || b <= 0 || b == 1) {
    //             r.errCode = ILLEGAL_PARAM_ERROR_CODE;
    //             r.errMsg = "Illegal param";
    //             return ArithmeticNumber();
    //         }
    //
    //         // 先计算近似值
    //         auto approx = boost::multiprecision::log(x.getValue()) / boost::multiprecision::log(b.getValue());
    //         // 取整后验证：b^n 是否等于x
    //         int n = static_cast<int>(boost::multiprecision::round(approx));
    //         if (boost::multiprecision::pow(b.getValue(), n) == x.getValue()) {
    //             return ArithmeticNumber(n); // 验证通过，返回精确整数
    //         }
    //         return ArithmeticNumber(approx);
    //     };
    //     registGlobalArithmeticFormualFunction("log", func);
    // }
    // // auto it = h.validExpression(text);
    // auto it = h.invoke(text);
    // if (it.data.type == AFVT_NUMBER) {
    //     qDebug() << std::get<FloatNumber10>(it.data.param).toMinString();
    // } else if (it.data.type == AFVT_BOOL) {
    //     qDebug() << (std::get<bool>(it.data.param) ? true: false);
    // } else {
    //     qDebug() << "nothing";
    // }
    // qDebug().noquote() << it.errMsg;


    // 创建一个绘图窗口
    // JKQTPlotter* plotter = new JKQTPlotter();
    //
    // JKQTPDatastore* ds = plotter->getDatastore();
    //
    // // 3. 准备数据
    // // QList<double> xData = {1, 2, 3, 4, 5};
    // // QList<double> yData = {1, 4, 9, 16, 25};
    //
    // // 4. 将数据复制到仓库，并获取列索引 (size_t)
    // size_t columnX = ds->addColumn("X");
    // size_t columnY = ds->addColumn("Y");
    // ds->set(columnX, 0, 1);
    // ds->set(columnY, 0, 4);
    // // size_t columnX = ds->addColumn(xData, "x");
    // // size_t columnY = ds->addColumn(yData, "y");
    //
    // // 5. 创建曲线对象，并关联数据列
    // JKQTPXYLineGraph* graph = new JKQTPXYLineGraph(plotter);
    // graph->setXColumn(columnX); // 告诉曲线：X轴用哪一列
    // graph->setYColumn(columnY); // 告诉曲线：Y轴用哪一列
    // graph->setTitle("我的曲线");
    //
    // // 6. 将曲线添加到绘图板
    // plotter->addGraph(graph);
    //
    // // 7. 自动调整缩放以显示所有数据
    // plotter->zoomToFit();
    //
    // // 8. 显示
    // plotter->show();


    return a.exec();
}
