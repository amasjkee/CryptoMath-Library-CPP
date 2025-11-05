#pragma once

/**
 * @file core.hpp
 * @brief Главный заголовочный файл для основных алгебраических структур CryptoMath
 * 
 * Этот заголовочный файл включает все основные алгебраические структуры:
 * - Множества и отображения
 * - Отношения и отношения эквивалентности
 * - Операции с мощностью
 * - Группоиды, полугруппы, моноиды, группы
 * - Подгруппы, нормальные подгруппы, смежные классы
 * - Фактор-группы
 * - Порядок элементов и показатель группы
 * - Циклические группы
 * - Функция Эйлера
 */

// Этап 1: Основа
#include "core/set.hpp"
#include "core/mapping.hpp"
#include "core/relation.hpp"
#include "core/cardinality.hpp"

// Этап 2: Множества с одной операцией
#include "core/concepts.hpp"
#include "core/groupoid.hpp"
#include "core/semigroup.hpp"
#include "core/monoid.hpp"
#include "core/group.hpp"
#include "core/cayley_table.hpp"

// Этап 3: Подгруппы
#include "core/subgroup.hpp"
#include "core/normal_subgroup.hpp"
#include "core/coset.hpp"
#include "core/center.hpp"
#include "core/factor_group.hpp"

// Этап 4: Порядок элементов
#include "core/element_order.hpp"
#include "core/group_exponent.hpp"

// Этап 5: Циклические группы
#include "core/cyclic_group.hpp"
#include "core/euler_function.hpp"

