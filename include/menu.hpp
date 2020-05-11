#pragma once

#include <atomic>
#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace rc {
    class CommandLineMenu {
    public:
        using Handler = std::function<void()>;

        void add_item(const std::string &title, const Handler &handler) {
            _items.emplace_back(title, handler);
        }

        void run_loop() const {
            while (true && !_should_exit) {
                _print_items();
                std::cout << "Please enter your choice: ";
                decltype(_items)::size_type choice;
                std::cin >> choice;
                if (choice < 1 || choice > _items.size()) {
                    _print_empty_line();
                    std::cout << "Invalid choice! Please try again." << std::endl;
                    _print_empty_line();
                    continue;
                }

                _print_empty_line();
                const auto &[_, handler] = _items.at(choice - 1);
                handler();
                _print_empty_line();
            }
        }

        void exit_menu() {
            _should_exit = true;
        }

    private:
        std::vector<std::pair<std::string, Handler>> _items;
        std::atomic_bool _should_exit = false;

        void _print_items() const {
            decltype(_items)::size_type choice = 1;
            for (const auto &[title, _] : _items) {
                std::cout << (choice++) << ". " << title << std::endl;
            }
        }

        void _print_empty_line() const {
            std::cout << std::endl;
        }
    };
} // namespace rc
