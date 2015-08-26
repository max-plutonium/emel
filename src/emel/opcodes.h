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
#include <utility>

namespace emel {

using insn_type = std::uint32_t;

enum class opcode : unsigned char {
        spec, // Специальные команды
        pop, // Снять значение с вершины стека
        dup, // Дублировать значение на вершине стека
        swap, // Обменять значения на вершине стека и значение под ней
        push_const, // Загрузить константу из пула в стек
 /*5*/  push_local, // Положить в стек локальную переменную
        push_field, // Положить в стек значение поля объекта
        push_static, // Положить в стек значение статического поля объекта
        load_local, // Загрузить значение из стека в локальную переменную
        load_field, // Загрузить значение из стека в поле объекта
 /*10*/ load_static, // Загрузить значение из стека в статическое поле объекта
        push_frame, // Создать новый кадр
        drop_frame, // Выйти из текущего кадра
        throw_, // Бросить исключение
        try_, // Начало блока try
 /*15*/ end_try, // Конец блока try
        un_op, // Унарная операция
        bin_op, // Бинарная операция
        call, // Вызов функции по имени
        fcall, // Вызов функции по номеру
 /*20*/ init, // Вызов функции инициализации объекта
        fini, // Вызов функции финализации объекта
        br, // Безусловный переход
        br_true, // Переход, если на вершине стека true
        br_false, // Переход, если на вершине стека false
 /*25*/ ret, // Выход из функции
};

enum class op_kind {
    or_ = 201, xor_, and_, eq, ne, lt, gt, lte, gte, add, sub, mul, div,
    not_ = 101, neg
};

std::pair<opcode, std::uint32_t> insn_decode(insn_type insn);

insn_type insn_encode(opcode op, std::uint32_t idx = 0);

} // namespace emel

#endif // OPCODES_H
