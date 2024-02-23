#include <src/application.h>

using namespace Oimo;

class TimerService : public Service {
public:
    void init(Packle::sPtr packle) override {
        LOG_INFO << "begin test timer";
        sleep(2000);
        LOG_INFO << "sleep 2000ms";
        addTimer(4000, 0, [this](){
            LOG_INFO << "timer 1 begin";
            auto id = addTimer(0, 3000, [](){
                LOG_INFO << "print per 3 seconds";
            });
            sleep(20000);
            LOG_INFO << "timer 1 end";
            removeTimer(id);
            sleep(5000);
            // APP::instance().stop();
        });
        // LOG_INFO << "end test timer";
        // addTimer(0, 5000, [](){});
        // test();
    }
    // void test() {
    //     tid = addTimer(0, 5000, [this](){
    //         close();
    //         LOG_DEBUG << "timer 1 end";
    //     });
    // }
    // void close() {
    //     removeTimer(tid);
    // }
    uint64_t tid;
};

int main() {
    auto& app = APP::instance();
    app.init();
    Logger::setLogLevel(LogLevel::DEBUG);
    app.newService<TimerService>("TimerService");
    return app.run();
}

