#pragma once

#include "groupoid.hpp"
#include "group.hpp"
#include <vector>
#include <map>
#include <iomanip>
#include <sstream>

namespace cryptomath {

/**
 * @brief Таблица Кэли для группоида/группы
 * 
 * Таблица Кэли - это таблица, описывающая бинарную операцию конечной
 * алгебраической структуры. Таблица перечисляет все возможные произведения элементов.
 */
template<typename T, typename Op>
class CayleyTable {
public:
    using element_type = T;
    using operation_type = Op;
    using table_type = std::map<std::pair<T, T>, T>;

    /**
     * @brief Построить таблицу Кэли из группоида
     */
    template<typename Structure>
        requires std::derived_from<Structure, Groupoid<T, Op>> && GroupoidConcept<typename Structure::element_type, typename Structure::operation_type>
    CayleyTable(const Structure& structure)
        : elements_(structure.get_set().begin(), structure.get_set().end()),
          operation_(structure.get_operation()) {
        
        // Строим таблицу
        for (const auto& a : elements_) {
            for (const auto& b : elements_) {
                table_[std::make_pair(a, b)] = structure.operate(a, b);
            }
        }
    }

    /**
     * @brief Получить результат операции a ∘ b из таблицы
     */
    T lookup(const T& a, const T& b) const {
        auto it = table_.find(std::make_pair(a, b));
        if (it == table_.end()) {
            throw std::domain_error("Elements not in Cayley table");
        }
        return it->second;
    }

    /**
     * @brief Получить все элементы в таблице
     */
    const std::vector<T>& get_elements() const noexcept {
        return elements_;
    }

    /**
     * @brief Получить базовую таблицу
     */
    const table_type& get_table() const noexcept {
        return table_;
    }

    /**
     * @brief Проверить, представляет ли таблица ассоциативную операцию
     */
    bool is_associative() const {
        for (const auto& a : elements_) {
            for (const auto& b : elements_) {
                for (const auto& c : elements_) {
                    T left = lookup(lookup(a, b), c);
                    T right = lookup(a, lookup(b, c));
                    if (left != right) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    /**
     * @brief Проверить, представляет ли таблица коммутативную операцию
     */
    bool is_commutative() const {
        for (const auto& a : elements_) {
            for (const auto& b : elements_) {
                if (lookup(a, b) != lookup(b, a)) {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * @brief Найти единичный элемент из таблицы
     * 
     * @return Единичный элемент, если найден, или выбрасывает исключение, если его нет
     */
    T find_identity() const {
        for (const auto& candidate : elements_) {
            bool is_identity = true;
            for (const auto& a : elements_) {
                if (lookup(candidate, a) != a || lookup(a, candidate) != a) {
                    is_identity = false;
                    break;
                }
            }
            if (is_identity) {
                return candidate;
            }
        }
        throw std::logic_error("No identity element found");
    }

    /**
     * @brief Проверить, имеет ли таблица свойство левой сократимости
     */
    bool has_left_cancellation() const {
        for (const auto& a : elements_) {
            std::map<T, T> seen;
            for (const auto& b : elements_) {
                T result = lookup(a, b);
                auto it = seen.find(result);
                if (it != seen.end() && it->second != b) {
                    return false; // Два разных b отображаются в один результат
                }
                seen[result] = b;
            }
        }
        return true;
    }

    /**
     * @brief Проверить, имеет ли таблица свойство правой сократимости
     */
    bool has_right_cancellation() const {
        for (const auto& b : elements_) {
            std::map<T, T> seen;
            for (const auto& a : elements_) {
                T result = lookup(a, b);
                auto it = seen.find(result);
                if (it != seen.end() && it->second != a) {
                    return false; // Два разных a отображаются в один результат
                }
                seen[result] = a;
            }
        }
        return true;
    }

    /**
     * @brief Проверить, имеет ли таблица оба свойства сократимости
     */
    bool has_cancellation() const {
        return has_left_cancellation() && has_right_cancellation();
    }

    /**
     * @brief Преобразовать таблицу в строковое представление
     * 
     * Создает форматированную таблицу, показывающую все операции
     */
    template<typename ToString>
    std::string to_string(ToString to_str = ToString{}) const {
        std::ostringstream oss;
        
        // Вычисляем ширину столбца
        size_t col_width = 4;
        for (const auto& elem : elements_) {
            std::string str = to_str(elem);
            col_width = std::max(col_width, str.length() + 2);
        }

        // Печатаем строку заголовка
        oss << std::setw(col_width) << "∘";
        for (const auto& elem : elements_) {
            oss << std::setw(col_width) << to_str(elem);
        }
        oss << "\n";

        // Печатаем строки таблицы
        for (const auto& row_elem : elements_) {
            oss << std::setw(col_width) << to_str(row_elem);
            for (const auto& col_elem : elements_) {
                T result = lookup(row_elem, col_elem);
                oss << std::setw(col_width) << to_str(result);
            }
            oss << "\n";
        }

        return oss.str();
    }

    /**
     * @brief Получить размер таблицы (количество элементов)
     */
    size_t size() const noexcept {
        return elements_.size();
    }

private:
    std::vector<T> elements_;
    Op operation_;
    table_type table_;
};

} // namespace cryptomath

