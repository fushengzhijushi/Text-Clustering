#include <float.h>
#include <math.h>

#include "gamma.h"

/* 欧拉伽马常数 */
#define GAMMA 0.577215664901532860606512090

#define HALF_LOG2_PI 0.91893853320467274178032973640562


static double lda_gamma(double x)
{
    /*
     * 将函数可行域拆成三段:
     * (0, 0.001), [0.001, 12), 和 (12, 无穷大).
     */

    /*
     * 第一段: (0, 0.001).
     */
    if (x < 0.001)
        return 1.0 / (x * (1.0 + GAMMA * x));

    /*
     * 第二段: [0.001, 12).
     */
    if (x < 12.0) {
        
        static const double p[] = {
            -1.71618513886549492533811E+0,
             2.47656508055759199108314E+1,
            -3.79804256470945635097577E+2,
             6.29331155312818442661052E+2,
             8.66966202790413211295064E+2,
            -3.14512729688483675254357E+4,
            -3.61444134186911729807069E+4,
             6.64561438202405440627855E+4
        };

        
        static const double q[] = {
            -3.08402300119738975254353E+1,
             3.15350626979604161529144E+2,
            -1.01515636749021914166146E+3,
            -3.10777167157231109440444E+3,
             2.25381184209801510330112E+4,
             4.75584627752788110767815E+3,
            -1.34659959864969306392456E+5,
            -1.15132259675553483497211E+5
        };

        double den, num, result, z;

        
        double y = x;
        int i, n = 0;
        int arg_was_less_than_one = (y < 1.0);

        
        if (arg_was_less_than_one)
            y += 1.0;
        else {
            n = (int)floor(y) - 1;
            y -= n;
        }

        num = 0.0;
        den = 1.0;

        z = y - 1;
        for (i = 0; i < 8; i++) {
            num = (num + p[i]) * z;
            den = den * z + q[i];
        }
        result = num/den + 1.0;

        
        if (arg_was_less_than_one)
            /* 使用等式 gamma(z) = gamma(z+1)/z*/
            result /= (y-1.0);
        else
            /* 使用等式 gamma(z+n) = z*(z+1)* ... *(z+n-1)*gamma(z) */
            for (i = 0; i < n; i++, y++)
                result *= y;

        return result;
    }

    /*
     * 第三段: [12, infinity).
     */
    if (x > 171.624)
        
        return 2 * DBL_MAX;
    return exp(lda_lgamma(x));
}


double lda_lgamma(double x)
{
    
    static const double c[8] =
    {
         1.0/12.0,
        -1.0/360.0,
         1.0/1260.0,
        -1.0/1680.0,
         1.0/1188.0,
        -691.0/360360.0,
         1.0/156.0,
        -3617.0/122400.0
    };

    double z, sum;
    int i;

    if (x < 12.0)
        return log(fabs(lda_gamma(x)));

    z = 1.0 / (x * x);
    sum = c[7];
    for (i=6; i >= 0; i--) {
        sum *= z;
        sum += c[i];
    }

    return (x - 0.5) * log(x) - x + HALF_LOG2_PI + sum / x;
}

