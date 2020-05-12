#pragma once

#include <iostream>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace rc {
    template <typename T>
    auto myers_diff(const std::vector<T> &src, const std::vector<T> &dst) {
        const auto src_size = src.size();
        const auto dst_size = dst.size();

        using Point = std::pair<size_t, size_t>;
        struct Head {
            Head(Point pos_, std::shared_ptr<Head> parent_ = nullptr)
                : pos(std::move(pos_)), parent(std::move(parent_)) {
            }
            Point pos;
            std::shared_ptr<Head> parent;
        };
        std::set<Point> point_set;
        std::queue<std::shared_ptr<Head>> head_queue;

        const auto zero_cost_shift = [&](Point &p) {
            auto &[x, y] = p;
            while (x < src_size && y < dst_size && src[x] == dst[y]) {
                x++;
                y++;
            }
        };
        const auto push_head = [&](std::shared_ptr<Head> head) {
            if (point_set.count(head->pos) == 0) {
                point_set.insert(head->pos);
                head_queue.push(head);
            }
        };
        const auto pop_head = [&] {
            auto head = head_queue.front();
            head_queue.pop();
            point_set.erase(head->pos);
            return head;
        };
        const auto is_done = [&](const Point &p) { return p == Point{src_size, dst_size}; };

        auto head = std::make_shared<Head>(Point{0, 0}, nullptr);
        zero_cost_shift(head->pos);
        push_head(head);
        while (!head_queue.empty()) {
            head = pop_head();
            const auto [x, y] = head->pos;
            if (x < src_size) {
                auto new_head = std::make_shared<Head>(Point{x + 1, y}, head);
                zero_cost_shift(new_head->pos);
                if (is_done(new_head->pos)) {
                    head = new_head;
                    break;
                }
                push_head(new_head);
            }
            if (y < dst_size) {
                auto new_head = std::make_shared<Head>(Point{x, y + 1}, head);
                zero_cost_shift(new_head->pos);
                if (is_done(new_head->pos)) {
                    head = new_head;
                    break;
                }
                push_head(new_head);
            }
        }

        // reverse the backtrace path
        std::vector<Point> diff_path;
        while (head) {
            diff_path.push_back(head->pos);
            head = head->parent;
        }
        std::reverse(diff_path.begin(), diff_path.end());
        return diff_path;
    }

    template <typename T>
    void print_diff_path(const std::vector<T> &src, const std::vector<T> &dst,
                         const decltype(myers_diff(src, dst)) &diff_path) {
        size_t lastx = 0, lasty = 0;
        for (size_t i = 0; i < diff_path.size(); i++) {
            const auto [x, y] = diff_path[i];
            const auto dx = x - lastx, dy = y - lasty;
            if (dx > dy) {
                std::cout << "- " << src[lastx] << std::endl;
                lastx++;
            } else if (dx < dy) {
                std::cout << "+ " << dst[lasty] << std::endl;
                lasty++;
            }
            while (lastx < x && lasty < y) {
                std::cout << "  " << src[lastx] << std::endl;
                lastx++;
                lasty++;
            }
        }
    }

    inline void test_diff_module() {
        {
            const std::string src_str = "ABCABBA";
            const std::string dst_str = "CBABAC";
            const std::vector<char> src(src_str.cbegin(), src_str.cend());
            const std::vector<char> dst(dst_str.cbegin(), dst_str.cend());
            const auto diff_path = myers_diff(src, dst);
            print_diff_path(src, dst, diff_path);
        }
        {
            const std::vector<std::string> src{
                "#include <stdio.h>",
                "int main() {",
                "    printf(\"Hello, world!\");",
                "    return 0;",
                "}",
            };
            const std::vector<std::string> dst{
                "#include <stdio.h>",
                "int main(int argc, char *argv[]) {",
                "    if (argc > 1)",
                "        printf(\"Hello, %s\", argv[1]);",
                "    else",
                "        printf(\"Hello, world!\");",
                "    return 0;",
                "}",
            };
            print_diff_path(src, dst, myers_diff(src, dst));
        }
    }
} // namespace rc
