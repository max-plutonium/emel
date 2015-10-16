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

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_LIST_SIZE 30
#define BOOST_MPL_LIMIT_VECTOR_SIZE 30

#include <boost/variant.hpp>

namespace emel {

using insn_type = std::uint32_t;

enum class opcode : unsigned char {
        spec, // Специальные команды
        pop, // Снять значение с вершины стека
        dup, // Дублировать значение на вершине стека
        swap, // Обменять значения на вершине стека и значение под ней
        push, // Положить в стек целое число
 /*5*/  push_const, // Загрузить константу из пула в стек
        push_local, // Положить в стек локальную переменную
        push_field, // Положить в стек значение поля объекта
        push_static, // Положить в стек значение статического поля объекта
        load_local, // Загрузить значение из стека в локальную переменную
 /*10*/ load_field, // Загрузить значение из стека в поле объекта
        load_static, // Загрузить значение из стека в статическое поле объекта
        push_frame, // Создать новый кадр
        drop_frame, // Выйти из текущего кадра
        raise, // Бросить исключение
 /*15*/ try_, // Начало блока try
        end_try, // Конец блока try
        call_op, // Вызов встроенной операции
        call, // Вызов функции по имени
        fcall, // Вызов функции по номеру
 /*20*/ call_static, // Вызов статической функции по имени
        fcall_static, // Вызов статической функции по номеру
        brf, // Безусловный переход вперед
        brb, // Безусловный переход назад
        brf_true, // Переход вперед, если на вершине стека true
 /*25*/ brf_false, // Переход вперед, если на вершине стека false
        brb_true, // Переход назад, если на вершине стека true
        brb_false, // Переход назад, если на вершине стека false
        br_table, // Создать таблицу переходов
        ret, // Возврат значения из функции
};

using insn_array = std::deque<insn_type>;
extern struct empty_value_type {} empty_value;
using value_type = boost::variant<empty_value_type, std::string, double, bool>;

enum class op_kind {
    not_ = 101, neg,
    or_ = 201, xor_, and_, eq, ne, lt, gt, lte, gte, add, sub, mul, div
};

std::pair<opcode, std::uint32_t> insn_decode(insn_type insn);

insn_type insn_encode(opcode op, std::uint32_t idx = 0);
insn_type insn_encode(opcode op, op_kind k);

const char *opcode_name(opcode op);
const char *opkind_name(op_kind op);

std::string insn_to_string(insn_type insn);

std::ostream &operator <<(std::ostream &os, empty_value_type);
std::ostream &operator <<(std::ostream &os, const insn_array &);

} // namespace emel

#endif // OPCODES_H
