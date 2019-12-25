#include "main.h"

int digit_at(int num, int k) {
	int len = 6; // fixed param for this use case
	return (num / (int)pow(10, len - k)) % 10;
}

bool eq(unsigned digit, char dgt) {
	return digit == (dgt - '0');
}

void break_pw() {
	// puzzle input:
	int rangeA = 125730; // start
	int rangeB = 579381; // end

	int possible_combinations_p1 = 0;
	int possible_combinations_p2 = 0;

	for (int i = rangeA; i <= rangeB; ++i) {
		bool has_double = false;
		bool has_exact_double = false;
		bool contiguous = true;
		for (int k = 2; k <= 6; ++k) {
			int digit_at_k0 = digit_at(i, k - 1);
			int digit_at_k1 = digit_at(i, k);

			if (digit_at_k0 > digit_at_k1) {
				contiguous = false;
				break;
			}
			if (digit_at_k0 == digit_at_k1) {
				has_double = true;
			}

			if (k == 2) {
				if (digit_at_k0 == digit_at_k1 && digit_at_k1 != digit_at(i, k + 1)) {
					has_exact_double = true;
					printf("l %d (%d), [%d %d] %d\n", i, k, digit_at_k0, digit_at_k1, digit_at(i, k + 1));
				}
			} else if (k == 6) {
				if (digit_at_k0 == digit_at_k1 && digit_at(i, k - 2) != digit_at_k0) {
					has_exact_double = true;
					printf("r %d (%d), %d [%d %d]\n", i, k, digit_at(i, k - 2), digit_at_k0, digit_at_k1);
				}
			} else {
				if (digit_at_k0 == digit_at_k1 && digit_at(i, k - 2) != digit_at_k0 && digit_at_k1 != digit_at(i, k + 1)) {
					has_exact_double = true;
					printf("m %d (%d), %d [%d %d] %d\n", i, k, digit_at(i, k - 2), digit_at_k0, digit_at_k1, digit_at(i, k + 1));
				}
			}
		}
		if (contiguous && has_double) {
			possible_combinations_p1++;
			if (has_exact_double) {
				possible_combinations_p2++;
			}
		}
	}

	printf("%d, %d\n", possible_combinations_p1, possible_combinations_p2);

	/*
	char digits[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};

	for (int k = 1; k <= 6; ++k) {
		int min_digit = 0;
		unsigned int digit_at_k = digit_at(rangeA, k);


		printf("%d, %d, %s\n", digit_at_k, ('1' - '0'), sbool(eq(digit_at_k, '1')));
	}*/
}

// here I was going to try to constructively count every combination, ruling some out early

// forget this code, I am currently too tired to jot down all the rules and exceptions to make this work
// so I will just let C do it's magic and brute force it...
int get_possible_combs(int min_digit, bool has_double, bool upper_limit, int pos, int upper, int lower) {
	int max_digit = 9;
	if (upper_limit) { // previous digits match upper limit, only go up to digit at upper
		max_digit = digit_at(upper, pos);
	}
	if (pos == 6) { // last digit
		if (has_double) {
			return max_digit - min_digit;
		} else {
			// we're on the last digit and it still needs a repeat to match the rules => only one possibility
			if (max_digit >= min_digit) {
				return 1;
			} else {
				return 0;
			}
		}
	} else {
		int possible_sub_combos = 0;
		for (int k = min_digit; k < max_digit; ++k) {
			int next_min = 00;
			possible_sub_combos += get_possible_combs(next_min, 0, 0, pos + 1, upper, lower);
		}
		return possible_sub_combos;
	}
}
