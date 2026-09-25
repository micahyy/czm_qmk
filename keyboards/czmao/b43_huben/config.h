/* Copyright 2025 MAOKB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifdef ENCODER_ENABLE
// Pins and per-encoder resolution now come from keyboard.json -> ENCODER_A_PINS /
// ENCODER_B_PINS / ENCODER_RESOLUTIONS. Do not redefine them here, the JSON based
// preprocessor generates them and duplicate definitions would clash.

// keypos_t is {col, row}. ENL/ENR of row 2 are the rotary-only positions.
#    define ENCODERS_MATRIX_MAP { {1, 2}, {0, 2} }
#endif
