#pragma once

#include <concepts>
#include <type_traits>

namespace cryptomath {

/**
 * @brief C++20 концепты для алгебраических структур
 * 
 * Эти концепты обеспечивают математические свойства алгебраических структур
 * на этапе компиляции, гарантируя типобезопасность и корректность.
 */

/**
 * @brief Концепт для бинарной операции на множестве
 * 
 * Бинарная операция - это функция, которая принимает два элемента и возвращает элемент
 */
template<typename Op, typename T>
concept BinaryOperation = std::regular_invocable<Op, T, T> &&
    std::same_as<std::invoke_result_t<Op, T, T>, T>;

/**
 * @brief Концепт для замкнутой бинарной операции
 * 
 * Операция является замкнутой, если применение её к любым двум элементам множества
 * производит элемент того же множества. Это неявно в нашей системе типов.
 */
template<typename Op, typename T>
concept ClosedBinaryOperation = BinaryOperation<Op, T>;

/**
 * @brief Концепт для ассоциативной операции
 * 
 * Операция является ассоциативной, если (a ∘ b) ∘ c = a ∘ (b ∘ c)
 * Это должно быть проверено во время выполнения или доказано математически.
 * Концепт обеспечивает проверку сигнатуры операции на этапе компиляции.
 */
template<typename Op, typename T>
concept AssociativeOperation = ClosedBinaryOperation<Op, T>;

/**
 * @brief Концепт для коммутативной операции
 * 
 * Операция является коммутативной, если a ∘ b = b ∘ a
 * Это должно быть проверено во время выполнения или доказано математически.
 */
template<typename Op, typename T>
concept CommutativeOperation = ClosedBinaryOperation<Op, T>;

/**
 * @brief Концепт для группоида
 * 
 * Группоид - это множество с замкнутой бинарной операцией.
 * Математически: (G, ∘), где G - множество и ∘: G × G → G
 */
template<typename T, typename Op>
concept GroupoidConcept = ClosedBinaryOperation<Op, T>;

/**
 * @brief Концепт для полугруппы
 * 
 * Полугруппа - это группоид с ассоциативной операцией.
 * Математически: (S, ∘), где S - множество, ∘ ассоциативна и замкнута.
 */
template<typename T, typename Op>
concept SemigroupConcept = GroupoidConcept<T, Op> && AssociativeOperation<Op, T>;

/**
 * @brief Концепт для моноида
 * 
 * Моноид - это полугруппа с единичным элементом.
 * Математически: (M, ∘, e), где M - полугруппа и e - единица: e ∘ a = a ∘ e = a
 */
template<typename T, typename Op>
concept MonoidConcept = SemigroupConcept<T, Op>;

/**
 * @brief Концепт для группы
 * 
 * Группа - это моноид, где каждый элемент имеет обратный.
 * Математически: (G, ∘, e), где G - моноид и для каждого a ∈ G
 * существует a⁻¹ ∈ G такое, что a ∘ a⁻¹ = a⁻¹ ∘ a = e
 */
template<typename T, typename Op>
concept GroupConcept = MonoidConcept<T, Op>;

/**
 * @brief Концепт для абелевой (коммутативной) группы
 * 
 * Абелева группа - это группа с коммутативной операцией.
 */
template<typename T, typename Op>
concept AbelianGroup = GroupConcept<T, Op> && CommutativeOperation<Op, T>;

/**
 * @brief Концепт для типа, который имеет единичный элемент
 * 
 * Этот концепт проверяет, может ли тип предоставить единичный элемент для операции.
 * Обычно реализуется структурами, представляющими моноиды или группы.
 */
template<typename T, typename Op>
concept HasIdentity = requires {
    { T::identity() } -> std::same_as<T>;
};

/**
 * @brief Концепт для типа, который может вычислять обратные элементы
 * 
 * Этот концепт проверяет, может ли тип вычислить обратный элемент.
 * Обычно реализуется структурами, представляющими группы.
 */
template<typename T, typename Op>
concept HasInverse = requires(const T& a) {
    { T::inverse(a) } -> std::same_as<T>;
};

/**
 * @brief Вспомогательный признак для проверки ассоциативности операции на этапе компиляции
 * 
 * Это заглушка - истинная ассоциативность должна быть доказана математически
 * или проверена во время выполнения. Этот признак может быть специализирован для известных ассоциативных операций.
 */
template<typename Op, typename T>
struct is_associative : std::false_type {};

/**
 * @brief Вспомогательный признак для проверки коммутативности операции на этапе компиляции
 * 
 * Это заглушка - истинная коммутативность должна быть доказана математически
 * или проверена во время выполнения. Этот признак может быть специализирован для известных коммутативных операций.
 */
template<typename Op, typename T>
struct is_commutative : std::false_type {};

template<typename Op, typename T>
inline constexpr bool is_associative_v = is_associative<Op, T>::value;

template<typename Op, typename T>
inline constexpr bool is_commutative_v = is_commutative<Op, T>::value;

} // namespace cryptomath

