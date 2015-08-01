/*
 * Copyright (C) 2015 Max Plutonium <plutonium.max@gmail.com>
 *
 * This file is part of the EMEL library.
 *
 * The EMEL library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * The EMEL library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the EMEL library. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef OPCODES_H
#define OPCODES_H

#include <cstdint>

namespace emel EMEL_EXPORT {

using insn_type = std::uint32_t;

enum class opcode : unsigned char {
    debug_break, // Точка останова
    pop, // Снять значение с вершины стека
    dup, // Дублировать значение на вершине стека
    swap, // Обменять значения на вершине стека и значение под ней
    push_const, // Загрузить константу из пула в стек
    push_local, // Положить в стек локальную переменную
    push_field, // Положить в стек значение поля объекта
    push_static, // Положить в стек значение статического поля объекта
    load_local, // Загрузить значение из стека в локальную переменную
    load_field, // Загрузить значение из стека в поле объекта
    load_static, // Загрузить значение из стека в статическое поле объекта
    push_frame, // Создать новый кадр
    drop_frame, // Выйти из текущего кадра
    throw_, // Бросить исключение
    try_, // Начало блока try
    end_try, // Конец блока try
    un_op, // Унарная операция
    bin_op, // Бинарная операция
    call, // Вызов метода объекта
    call_static, // Вызов статического метода объекта
    call_special, // Вызов специальной функции
    br, // Безусловный переход
    br_true, // Переход, если на вершине стека true
    br_false, // Переход, если на вершине стека false
    ret, // Выход из функции
};

} // namespace emel

extern "C" {

EMEL_EXPORT void
emel_insn_decode(emel::insn_type insn, emel::opcode *op, std::uint32_t *idx);

EMEL_EXPORT emel::insn_type
emel_insn_encode(emel::opcode op, std::uint32_t idx = 0);

} // extern "C"

#endif // OPCODES_H
