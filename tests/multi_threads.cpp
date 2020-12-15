#include <thread>

int main( ) {
    constexpr int num_thread = 10;
    std::thread t[num_thread];

    for (int i = 0; i < num_thread; i++) {
        t[i] = std::thread([i](){
            fprintf(stderr, "tid %d\n", i);
        });
    }

    for (int i = 0; i < num_thread; i++) {
        t[i].join();
    }
}
