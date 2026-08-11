#ifndef FIXED_POINT_H
#define FIXED_POINT_H


// Fixed-point arithmetic implementation for Pintos project

#define F (1 << 14) // Scaling factor for fixed-point representation

#define INT_TO_FP(n) ((n) * F) // Convert integer to fixed-point
#define FP_TO_INT_ROUND_ZERO(x) ((x) / F) // Convert fixed-point to integer (rounding toward zero)
#define FP_TO_INT_ROUND_NEAREST(x) (((x) >= 0) ? (((x) + F / 2) / F) : (((x) - F / 2) / F))
#define ADD_FP(x,y) ((x) + (y)) // Add two fixed-point numbers
#define SUB_FP(x,y) ((x) - (y)) // Subtract two fixed-point

#define ADD_MIX(x,n) ((x) + INT_TO_FP(n)) // Add fixed-point and integer
#define SUB_MIX(x,n) ((x) - INT_TO_FP(n)) // Subtract
#define MULT_MIX(x,n) ((x) * (n)) // Multiply fixed-point by integer
#define DIV_MIX(x,n) ((x) / (n)) // Divide fixed-point by integer

#define MULT_FP(x,y) (((int64_t)(x)) * (y) / F) // Multiply two fixed-point numbers
#define DIV_FP(x,y) (((int64_t)(x)) * F / (y))

#endif /* threads/fixed-point.h */