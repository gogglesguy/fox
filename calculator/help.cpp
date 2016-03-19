/********************************************************************************
*                                                                               *
*                      O n - L i n e   H e l p   T e x t                        *
*                                                                               *
*********************************************************************************
* Copyright (C) 2001,2008 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This program is free software: you can redistribute it and/or modify          *
* it under the terms of the GNU General Public License as published by          *
* the Free Software Foundation, either version 3 of the License, or             *
* (at your option) any later version.                                           *
*                                                                               *
* This program is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 *
* GNU General Public License for more details.                                  *
*                                                                               *
* You should have received a copy of the GNU General Public License             *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.         *
*********************************************************************************
* $Id: help.cpp,v 1.9 2008/01/04 15:18:13 fox Exp $                             *
********************************************************************************/
#include "fx.h"
#include "Calculator.h"


// Help text
const char Calculator::help[]=
"\n\n"
"                                       The FOX Calculator\n"
"\n\n"
"Introduction.\n"
"\n"
"The FOX Calculator is a simple desktop calculator geared toward the programmer.  "
"It supports not only a full complement scientific functions, but also common "
"operations that programmers need, such as bitwise operations, bitwise shifting, "
"and base-2 logarithm and exponents, and numeric conversion between hexadecimal, "
"octal, binary, and decimal.\n"
"The FOX Calculator implements correct operator precedences, so expressions like "
"2+3*5 yield the correct result, which is 17, and not 25.\n"
"Also featured is a constant memory, which permanently stores its value even if "
"you exit the calculator and restart it later.\n"
"\n\n"
"Configuration.\n"
"\n"
"Pressing on the calculator icon brings up the Calculator Preferences dialog.  "
"The Calculator Preferences dialog comprises three settings:\n\n"
"\t\t- Settings for the calculator itself;\n"
"\t\t- Color settings of the button groups;\n"
"\t\t- Information about the calculator.\n"
"\n"
"In the Calculator settings panel, you can change font used for the "
"display, by pressing the \"Set...\" button to bring up the standard Font Selection "
"Dialog.\n"
"You can change the way numbers are printed as well.  Checking \"Always show exponent\" "
"will cause the calculator display always to display the number in exponential notation.  "
"Checking \"Never show exponent\" will cause the calculator to render the number in simple "
"dot notation.\n"
"The precision can be set by means of the spin button; the default precision is set to 16.\n"
"Finally, the calculator can be set to beep when errors occur.\n"
"\n"
"In the Color settings panel, you can change the colors of the various button groups.\n"
"The buttons are grouped by function; the numbers are in one group, and the operators are "
"in another, and so on.\n"
"\n"
"In the About panel, some information is presented about the calculator, like version "
"number and author's contact.\n"
"\n\n"
"Entering Numbers.\n"
"\n"
"You can enter a number by clicking on the digit buttons, or simply hit the right "
"digit on the keyboard.  Numbers in exponential notation are entered by entering the "
"mantissa first, then hitting the \"EXP\" button, and then entering the exponent. "
"Up to 3 digits may be entered for the exponent; entering more than 3 will cause the "
"digits to shift, i.e. the first digit entered will be dropped and replaced by the "
"second, the second digit will be replaced by the third, and the third will be replaced "
"by the new input.\n"
"Changing the sign of the exponent is accomplished by hittin the \"+/-\" button.\n"
"At any time, you can hit the Backspace key to delete the last digit entered.\n"
"Two numbers, pi and e (euler's number) may be entered with a single button:\n"
"\n"
"\tpi\tEnters the number 3.1415926535897932384626433833\n"
"\te\tEnters the number 2.7182818284590452353602874713 (hit the \"inv\" button first)\n"
"\tphi\tEnters the golden ratio number 1.6180339887498948482045868343 (hit the \"hyp\" button first)\n"
"\t1/phi\tEnters the inverse golden ratio number (hit the \"hyp\" and \"inv\" buttons)\n"
"\n\n"
"Operators.\n"
"\n"
"The operators in the FOX Calculator are the usual suspects:\n"
"\n"
"\t+\tAddition\n"
"\t-\tSubstraction\n"
"\t*\tMultiplication\n"
"\t/\tFloating point division\n"
"\n"
"In addition, FOX Calculator also includes bitwise operators, such as:\n"
"\n"
"\tAND\tBit-wise logical and\n"
"\tOR\tBit-wise logical or\n"
"\tXOR\tBit-wise logical exclusive or\n"
"\tNOT\tBit-wise logical not\n"
"\tSHL\tBit-wise shift left\n"
"\tSHR\tBit-wise shift right\n"
"\tSAR\tBit-wise signed shift right (hit the \"inv\" button first)\n"
"\n"
"Also nice for programmers is the inclusion of integer operations:\n"
"\n"
"\tmod\tInteger modulo\n"
"\tdiv\tInteger division (hit the \"inv\" button first)\n"
"\n"
"All the operators have certain precedence relations with each other, so that "
"an expression is evaluated correctly.\n"
"\n\n"
"Trigonometric Functions.\n"
"\n"
"The Calculator incorporates the usual trigonometric functions:\n"
"\n"
"\tsin    \tSine\n"
"\tcos    \tCosine\n"
"\ttan    \tTangent\n"
"\tasin  \tInverse sine or arc sine (hit the \"inv\" button first)\n"
"\tacos  \tInverse cosine\n"
"\tatan  \tInverse tangent\n"
"\tsinh  \tHyperbolic sine (hit the \"hyp\" button first)\n"
"\tcosh  \tHyperbolic cosine\n"
"\ttanh  \tHyperbolic tangent\n"
"\tasinh \tInverse hyperbolic sine (hit the \"hyp\" and \"inv\"buttons first)\n"
"\tacosh \tInverse hyperbolic cosine\n"
"\tatanh \tInverse hyperbolic tangent\n"
"\n"
"For the first 6 functions, the angle mode determines whether the argument is "
"specified in terms of degrees, radians, or grad.  "
"Note that the angle mode is preserved across invocations of the Calculator.\n"
"\n\n"
"Other Functions.\n"
"\n"
"Other functions supported by the calculator are the following:\n"
"\n"
"\tlog     \tBase 10 logarithm\n"
"\tln      \tNatural logarithm\n"
"\t2log    \tBase 2 logarithm\n"
"\tx!      \tFactorial\n"
"\tnPr     \tPermutations\n"
"\tnCr     \tCombinations\n"
"\tsqrt    \tSquare root\n"
"\tx^y     \tX raised to the power y\n"
"\t1/x     \tReciprocal\n"
"\t10^x    \tBase 10 exponentiation (hit the \"inv\" button first)\n"
"\te^x     \tExponentiation\n"
"\t2^x     \tBase 2 exponentiation\n"
"\tx^1/y \tX raised to the power 1/y\n"
"\tx^2     \tX squared\n"
"\n\n"
"Limits.\n"
"\n"
"The calculator works in IEEE 746 double precision mode; for bit-wise operations, "
"it uses 32 bit integers.  Thus, the numeric limits are as follows: \n"
"\n"
"Smallest real number:\t2.2250738585072010e-308\n"
"Largest real number:  \t1.7976931348623158e+308\n"
"Smallest integer number:\t0\n"
"Largest integer number:\t4294967295\n"
;
