#include <src/application.h>

using namespace Oimo;

class TimerService : public Service {
public:
    void init(Packle::sPtr packle) override {
        LOG_INFO << "begin test timer";
        sleep(2000);
        LOG_INFO << "sleep 2000ms";
        addTimer(4000, 0, [this](){
            LOG_INFO << "timer 1";
            addTimer(0, 5000, [](){
                LOG_INFO << "print per 5 seconds";
            });
        });
        LOG_INFO << "end test timer";
        // addTimer(0, 5000, [](){});
    }
};

int main() {
    auto& app = APP::instance();
    app.init();
    // Logger::setLogLevel(LogLevel::DEBUG);
    app.newService<TimerService>("TimerService");
    return app.run();
}

