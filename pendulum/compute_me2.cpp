#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <thread>
#include <limits>
#include <cctype>

constexpr double INITIAL_X1 = 50.0;
constexpr double INITIAL_X2 = -20.0;
constexpr double TIME_STEP = 0.001;
constexpr double FAST_FORWARD_STEP = 0.05;


class SystemSimulator {
private:
    struct State {
        double x1, x2;
        double n1, n2;
        double u;
    };

    State current;
    double time = 0.0;

public:
    SystemSimulator() : current{INITIAL_X1, INITIAL_X2, INITIAL_X1, 0.0, 0.0} {}


    void update() {
        time += TIME_STEP;


        const double dx1 = current.x2;
        const double dx2 = 4.9 * current.x1 - 0.25 * current.u;
        const double du = 819.6 * current.n1 + 408.0 * current.n2;
        const double dn1 = 102.0 * (current.x1 - current.n1) + current.n2;
        const double dn2 = -200.0 * current.n1 + 204.9 * current.x1 - 0.25 * current.u;

        current.x1 += dx1 * TIME_STEP;
        current.x2 += dx2 * TIME_STEP;
        current.n1 += dn1 * TIME_STEP;
        current.n2 += dn2 * TIME_STEP;
        current.u = du;
    }

    void fastForward(double duration) {
        const int steps = static_cast<int>(duration / TIME_STEP);
        for (int i = 0; i < steps; ++i) {
            update();
        }
    }


    void reset() {
        current = {INITIAL_X1, INITIAL_X2, INITIAL_X1, 0.0, 0.0};
        time = 0.0;
    }


    void printState() const {
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Time: " << time << " s\n";
        std::cout << "System state: x1 = " << current.x1 << ", x2 = " << current.x2 << "\n";
        std::cout << "Observer state: n1 = " << current.n1 << ", n2 = " << current.n2 << "\n";
        std::cout << "Control: u = " << current.u << "\n\n";
    }


    double getTime() const { return time; }
};

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}


void showMenu() {
    std::cout << "\n=== Control Menu ===\n";
    std::cout << "1. Next step\n";
    std::cout << "2. Fast forward (0.05s)\n";
    std::cout << "3. Fast forward (3s)\n";
    std::cout << "4. Reset system\n";
    std::cout << "5. Exit\n";
    std::cout << "Enter choice: ";
}


int main() {
    SystemSimulator simulator;
    simulator.printState();

    while (true) {
        showMenu();

        char choice;
        std::cin >> choice;
        clearInput();

        switch (tolower(choice)) {
            case '1':
                simulator.update();
                break;

            case '2':
                simulator.fastForward(FAST_FORWARD_STEP);
                break;

            case '3':
                simulator.fastForward(3.0);
                break;

            case '4':
                simulator.reset();
                break;

            case '5':
                return 0;

            default:
                std::cout << "Invalid choice. Please try again.\n";
                continue;
        }

        simulator.printState();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
