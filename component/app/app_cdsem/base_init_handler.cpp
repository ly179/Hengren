#include "base_init_handler.h"

#include "app_util.h"
#include "cdsem_global_data.h"
#include "cdsem_control_helper.h"
#include "app_exit_thread_manager.h"
#include "app_thread_invoker.h"
#include "recipe_procedure.h"
#include "zeiss_tcp_client.h"
#include "cdsem_thread.h"
#include "sem_global.h"

#include "zeiss_client_proxy.h"
#include "arithmetic_formula_handler.h"
#include "image_type_defition.h"
#include "app_process_tower.h"

#define FORMULA_TRY try {

#define FORMULA_CATCH \
} catch(const std::exception& e) { \
    r.errCode = ILLEGAL_PARAM_ERROR_CODE; \
    r.errMsg = e.what(); \
} catch(...) { \
    r.errCode = UNKNOW_ERR_CODE; \
    r.errMsg = "Processing Formula unknow error occern"; \
} \
return ArithmeticNumber(); \

void startLogCollector() {
    CDSEM_Thread* thread = new CDSEM_Thread("App_Process_Tower_Thread");
    thread->start();
    AppProcessTower* tower = new AppProcessTower;
    tower->setThread(thread);
    tower->setTowerName("CD_SEM");
    tower->putProcess("LogCollector");
    tower->setTimeout(10000);
    AppProcessTower::connect(tower,
                             &AppProcessTower::processConnected,
                             tower,
                             [](const QString& name) {
                                 qDebug() << "process " << name << " online.";
                             });
    AppProcessTower::connect(tower,
                             &AppProcessTower::processDied,
                             tower,
                             [](const QString& name) {
                                 qDebug() << "process " << name << " offline.";
                                 QStringList args;
                                #if defined(__SOURCE_CODE_DIR) && defined(QT_DEBUG)
                                 args.reserve(2);
                                 args << "--path" << __SOURCE_CODE_DIR;
                                #endif
                                 startProcessBackground(__LOG_COLLECTOR_BIN, args);
                             });
    QObject::connect(thread, &QThread::finished, getCoreApp(), [tower, thread]() {
        AppExitThreadManager::instance()->finished(thread);
        delete tower;
        qDebug() << "tower deleted.";
    });
    AppExitThreadManager::instance()->setExitAndAutoCall(thread, [tower](QThread* t) {
        tower->close();
    });
}

void initFormulaVariable() {
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(2);
        func.types.push_back(AFVT_NUMBER);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return floatNumberPow(std::get<ArithmeticNumber>(params[0]), std::get<ArithmeticNumber>(params[1]));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("pow", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult&){
            return ArithmeticNumber("3.14159265358979");
        };
        registGlobalArithmeticFormualFunction("pi", func);
        registGlobalArithmeticFormualVariable("$pi", ArithmeticFormulaVariable {ArithmeticNumber("3.14159265358979"), AFVT_NUMBER});
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(1);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return floatNumberAbs(std::get<ArithmeticNumber>(params[0]));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("abs", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(1);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return ArithmeticNumber(boost::multiprecision::sin(std::get<ArithmeticNumber>(params[0]).getValue()));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("sin", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(1);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return ArithmeticNumber(boost::multiprecision::cos(std::get<ArithmeticNumber>(params[0]).getValue()));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("cos", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(1);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return ArithmeticNumber(boost::multiprecision::tan(std::get<ArithmeticNumber>(params[0]).getValue()));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("tan", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(1);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return ArithmeticNumber(boost::multiprecision::sinh(std::get<ArithmeticNumber>(params[0]).getValue()));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("sinh", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(1);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return ArithmeticNumber(boost::multiprecision::cosh(std::get<ArithmeticNumber>(params[0]).getValue()));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("cosh", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(1);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return ArithmeticNumber(boost::multiprecision::tanh(std::get<ArithmeticNumber>(params[0]).getValue()));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("tanh", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(1);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return ArithmeticNumber(boost::multiprecision::asin(std::get<ArithmeticNumber>(params[0]).getValue()));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("asin", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(1);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return ArithmeticNumber(boost::multiprecision::asinh(std::get<ArithmeticNumber>(params[0]).getValue()));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("asinh", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(1);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return ArithmeticNumber(boost::multiprecision::acos(std::get<ArithmeticNumber>(params[0]).getValue()));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("acos", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(1);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return ArithmeticNumber(boost::multiprecision::acosh(std::get<ArithmeticNumber>(params[0]).getValue()));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("acosh", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(1);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return ArithmeticNumber(boost::multiprecision::atan(std::get<ArithmeticNumber>(params[0]).getValue()));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("atan", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(1);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return ArithmeticNumber(boost::multiprecision::atanh(std::get<ArithmeticNumber>(params[0]).getValue()));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("atanh", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(1);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return ArithmeticNumber(boost::multiprecision::sqrt(std::get<ArithmeticNumber>(params[0]).getValue()));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("sqrt", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(2);
        func.types.push_back(AFVT_NUMBER);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            const ArithmeticNumber& x = std::get<ArithmeticNumber>(params[0]);
            const ArithmeticNumber& n = std::get<ArithmeticNumber>(params[1]);
            ArithmeticNumber v = ArithmeticNumber(1) / n;
            ArithmeticNumber nr = floatNumberPow(x, v);
            ArithmeticNumber tmp = ArithmeticNumber(boost::multiprecision::round(nr.getValue()));
            if (x < 0) {
                ArithmeticNumber abs_x = floatNumberAbs(x);
                nr = floatNumberPow(abs_x, v);
                nr = ArithmeticNumber(-nr.getValue()); // 补回负号
            } else {
                nr = floatNumberPow(x, v);
            }

            if (floatNumberAbs(floatNumberPow(tmp, n) - x).getValue() < 1e-15) {
                return tmp;
            }
            return nr;
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("sqrt", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(1);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return ArithmeticNumber(boost::multiprecision::log10(std::get<ArithmeticNumber>(params[0]).getValue()));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("log10", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(1);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return ArithmeticNumber(boost::multiprecision::log2(std::get<ArithmeticNumber>(params[0]).getValue()));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("log2", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(1);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            FORMULA_TRY
            return ArithmeticNumber(boost::multiprecision::log(std::get<ArithmeticNumber>(params[0]).getValue()));
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("log", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(2);
        func.types.push_back(AFVT_NUMBER);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            const ArithmeticNumber& b = std::get<ArithmeticNumber>(params[1]);
            const ArithmeticNumber& x = std::get<ArithmeticNumber>(params[0]);
            FORMULA_TRY
            // 先计算近似值
            auto approx = boost::multiprecision::log(x.getValue()) / boost::multiprecision::log(b.getValue());
            // 取整后验证：b^n 是否等于x
            int n = static_cast<int>(boost::multiprecision::round(approx));
            if (boost::multiprecision::pow(b.getValue(), n) == x.getValue()) {
                return ArithmeticNumber(n); // 验证通过，返回精确整数
            }
            return ArithmeticNumber(approx);
            FORMULA_CATCH
        };
        registGlobalArithmeticFormualFunction("log", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(2);
        func.types.push_back(AFVT_NUMBER);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            const ArithmeticNumber& a = std::get<ArithmeticNumber>(params[0]);
            const ArithmeticNumber& b = std::get<ArithmeticNumber>(params[1]);
            return a > b ? a:b;
        };
        registGlobalArithmeticFormualFunction("max", func);
    }
    {
        ArithmeticFormulaFunction func;
        func.types.reserve(2);
        func.types.push_back(AFVT_NUMBER);
        func.types.push_back(AFVT_NUMBER);
        func.returnType = AFVT_NUMBER;
        func.func = [](const std::vector<ArithmeticFormualParam>& params, OperationResult& r){
            const ArithmeticNumber& a = std::get<ArithmeticNumber>(params[0]);
            const ArithmeticNumber& b = std::get<ArithmeticNumber>(params[1]);
            return a < b ? a:b;
        };
        registGlobalArithmeticFormualFunction("min", func);
    }
}

void initBaseServer() {
    startLogCollector();
    QThread* t = getMainThread();
    QObject::connect(&STAGE_ENV, &CDSEM_GlobalStagePyParamModel::movingStageRotationChanged, t, []() {
        CDSEM_ControlHelper::instance()->rotation(nullptr, SEM_PY_ENV.getBeamRot());
    });

    {
        AppThreadInvoker* invoker = cdsem::CDSEM_GlobalDataHelper::get()->imageInvoker();
        AppExitThreadManager::setExitAndAutoCall(invoker->thread(), [invoker](QThread*) {
            invoker->quit();
        });
    }
    AppExitThreadManager::setExitAndAutoCall(RecipeProcedureHelper::get()->innerThread(), [](QThread*) {
        RecipeProcedureHelper::get()->exit();
    });
    AppExitThreadManager::setExitAndAutoCall(ZeissTcpClientHelper::get()->thread(), [](QThread*) {
        ZeissClientProxy::instance()->exit();
    });

    //CDSEM_ControlHelper属于TCP子线程对象，imageOperationController属于UI线程对象。
    //writeImageFromSupplier函数的设计用意就是允许在单一子线程上高速刷新图像，通知UI线程绘图的。
    //CDSEM_ControlHelper的线程已经注册到AppThreadManager中了，当前的条件是只有注册到里面的所有线程都停止了，才会退出QApplication。
    //而imageOperationController不是closeOnDelete，而且还是全局对象，也就QApplication退出后才会销毁
    //如此保证两个不同的线程上的对象的生命周期是安全的。
    QObject::connect(CDSEM_ControlHelper::instance()->getControl(),
                     &CDSEM_Control::imageReady, CDSEM_ControlHelper::instance()->getControl(), [](const QByteArray& data, const QSize& imageSize, const QSize& originSize, cdsem::ScanImageType type){
                         cdsem::CDSEM_GlobalDataHelper::get()->invokeImage([&originSize, &type, &imageSize, &data](){
                             ImageAddtionDataModel addition;
                             addition = SEM_PY_ENV;
                             addition = STAGE_ENV;
                             addition.originSize = originSize;
                             addition.imageType = type;
                             addition.realSize = imageSize;
                             addition.photoImagetype = cdsem::PIT_SEM;
                             addition.waferPos = cdsem::WaferStageCoordinateConvert::instance()->waferPosM(true);
                             cdsem::CDSEM_GlobalDataHelper::get()->setImage(IMG_OPT_TYPE_SEM, data, imageSize.width(), imageSize.height(), int(QImage::Format_Grayscale8), addition);
                         }, true);
                     }, Qt::DirectConnection);
    cdsem::CDSEM_GlobalDataHelper::get()->registImageType(IMG_OPT_TYPE_SEM);
    cdsem::CDSEM_GlobalDataHelper::get()->registImageType(IMG_OPT_TYPE_IM);

    initFormulaVariable();
}
