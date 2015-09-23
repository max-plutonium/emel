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
#include <deque>
#include <utility>
#include <boost/variant.hpp>

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
        call_op, // Вызов встроенной операции
        call, // Вызов функции по имени
        fcall, // Вызов функции по номеру
        call_static, // Вызов статической функции по имени
 /*20*/ fcall_static, // Вызов статической функции по номеру
        brf, // Безусловный переход вперед
        brb, // Безусловный переход назад
        brf_true, // Переход вперед, если на вершине стека true
        brf_false, // Переход вперед, если на вершине стека false
 /*25*/ brb_true, // Переход назад, если на вершине стека true
        brb_false, // Переход назад, если на вершине стека false
        ret, // Возврат значения из функции
};

using insn_array = std::deque<insn_type>;
using value_type = boost::variant<std::string, double, bool>;

enum class op_kind {
    or_ = 201, xor_, and_, eq, ne, lt, gt, lte, gte, add, sub, mul, div,
    not_ = 101, neg
};

std::pair<opcode, std::uint32_t> insn_decode(insn_type insn);

insn_type insn_encode(opcode op, std::uint32_t idx = 0);

const char *opcode_name(opcode op);

std::string insn_to_string(insn_type insn);

} // namespace emel

#endif // OPCODES_H
