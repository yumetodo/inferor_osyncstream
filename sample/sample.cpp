#include <inferior/osyncstream.hpp>
#include <iostream>

void thread1(int value) {
    // 下記行の処理結果がアトミックにストリーム出力される
    inferior::osyncstream(std::cout) << "value=" << value << '\n';
}

void thread2(int value) {
    // 同期化出力ストリームオブジェクトを作成
    inferior::osyncstream syncout{std::cout};

    syncout << "value=" << value << std::endl;
    syncout.emit();  // emit呼び出しでアトミックにストリーム出力される

    syncout << "double=" << (value * 2) << std::endl;
    // osyncstreamデストラクタにより暗黙にemitが呼び出される
}
int main() {
    thread1(4);
    thread2(3);
}
