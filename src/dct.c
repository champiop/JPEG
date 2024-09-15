#include "dct.h"

void Offset(size_t size, uint8_t *channel)
{
    for (size_t i = 0; i < size; i++)
    {
        channel[i] -= 128;
    }
}

// Inspiré par l'algorithme du site nayuki.io
// DCT de type 2 par Arai, Agui, Nakajima, 1988
// C'est une DCT en 1 dimension appliquée pour les 8 colonnes et les 8 lignes
void DCT8x8(float *dest, int8_t *src)
{
    for (int y = 0; y < 8; y++)
    {

        float v0 = src[y + 8 * 0] + src[y + 8 * 7];
        float v1 = src[y + 8 * 1] + src[y + 8 * 6];
        float v2 = src[y + 8 * 2] + src[y + 8 * 5];
        float v3 = src[y + 8 * 3] + src[y + 8 * 4];
        float v4 = src[y + 8 * 3] - src[y + 8 * 4];
        float v5 = src[y + 8 * 2] - src[y + 8 * 5];
        float v6 = src[y + 8 * 1] - src[y + 8 * 6];
        float v7 = src[y + 8 * 0] - src[y + 8 * 7];

        float v8 = v0 + v3;
        float v9 = v1 + v2;
        float v10 = v1 - v2;
        float v11 = v0 - v3;
        float v12 = -v4 - v5;
        float v13 = (v5 + v6) * 0.707106781186547524400844;
        float v14 = v6 + v7;

        float v15 = v8 + v9;
        float v16 = v8 - v9;
        float v17 = (v10 + v11) * 0.707106781186547524400844;
        float v18 = (v12 + v14) * 0.382683432365089771728460;

        float v19 = -v12 * 0.541196100146196984399723 - v18;
        float v20 = v14 * 1.306562964876376527856643 - v18;

        float v21 = v17 + v11;
        float v22 = v11 - v17;
        float v23 = v13 + v7;
        float v24 = v7 - v13;

        float v25 = v19 + v24;
        float v26 = v23 + v20;
        float v27 = v23 - v20;
        float v28 = v24 - v19;

        dest[y + 8 * 0] = 0.353553390593273762200422 * v15;
        dest[y + 8 * 1] = 0.254897789552079584470970 * v26;
        dest[y + 8 * 2] = 0.270598050073098492199862 * v21;
        dest[y + 8 * 3] = 0.300672443467522640271861 * v28;
        dest[y + 8 * 4] = 0.353553390593273762200422 * v16;
        dest[y + 8 * 5] = 0.449988111568207852319255 * v25;
        dest[y + 8 * 6] = 0.653281482438188263928322 * v22;
        dest[y + 8 * 7] = 1.281457723870753089398043 * v27;
    }

    for (int x = 0; x < 8; x++)
    {
        float v0 = dest[0 + 8 * x] + dest[7 + 8 * x];
        float v1 = dest[1 + 8 * x] + dest[6 + 8 * x];
        float v2 = dest[2 + 8 * x] + dest[5 + 8 * x];
        float v3 = dest[3 + 8 * x] + dest[4 + 8 * x];
        float v4 = dest[3 + 8 * x] - dest[4 + 8 * x];
        float v5 = dest[2 + 8 * x] - dest[5 + 8 * x];
        float v6 = dest[1 + 8 * x] - dest[6 + 8 * x];
        float v7 = dest[0 + 8 * x] - dest[7 + 8 * x];

        float v8 = v0 + v3;
        float v9 = v1 + v2;
        float v10 = v1 - v2;
        float v11 = v0 - v3;
        float v12 = -v4 - v5;
        float v13 = (v5 + v6) * 0.707106781186547524400844;
        float v14 = v6 + v7;

        float v15 = v8 + v9;
        float v16 = v8 - v9;
        float v17 = (v10 + v11) * 0.707106781186547524400844;
        float v18 = (v12 + v14) * 0.382683432365089771728460;

        float v19 = -v12 * 0.541196100146196984399723 - v18;
        float v20 = v14 * 1.306562964876376527856643 - v18;

        float v21 = v17 + v11;
        float v22 = v11 - v17;
        float v23 = v13 + v7;
        float v24 = v7 - v13;

        float v25 = v19 + v24;
        float v26 = v23 + v20;
        float v27 = v23 - v20;
        float v28 = v24 - v19;

        dest[0 + 8 * x] = 0.353553390593273762200422 * v15;
        dest[1 + 8 * x] = 0.254897789552079584470970 * v26;
        dest[2 + 8 * x] = 0.270598050073098492199862 * v21;
        dest[3 + 8 * x] = 0.300672443467522640271861 * v28;
        dest[4 + 8 * x] = 0.353553390593273762200422 * v16;
        dest[5 + 8 * x] = 0.449988111568207852319255 * v25;
        dest[6 + 8 * x] = 0.653281482438188263928322 * v22;
        dest[7 + 8 * x] = 1.281457723870753089398043 * v27;
    }
}