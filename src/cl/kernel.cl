typedef struct {
  float R;
  float G;
  float B;
  float A;
} RGBA;

typedef struct {
  float R;
  float G;
  float B;
} RGB;

typedef struct {
  unsigned char R;
  unsigned char G;
  unsigned char B;
  unsigned char A;
} RGBAOrigin;

typedef struct {
  unsigned char R;
  unsigned char G;
  unsigned char B;
} RGBOrigin;

inline RGB vecadd(RGB a, RGB b) {
  RGB res;
  res.R = a.R + b.R;
  res.G = a.G + b.G;
  res.B = a.B + b.B;
  return res;
}

inline RGB vecsub(RGB a, RGB b) {
  RGB res;
  res.R = a.R - b.R;
  res.G = a.G - b.G;
  res.B = a.B - b.B;
  return res;
}

inline RGB vecaddn(RGB a, float b) {
  RGB res;
  res.R = a.R + b;
  res.G = a.G + b;
  res.B = a.B + b;
  return res;
}

inline RGB vecmn(RGB a, float b) {
  RGB res;
  res.R = a.R * b;
  res.G = a.G * b;
  res.B = a.B * b;
  return res;
}

#define SCALE 1.0f / 255.0f

inline RGB fori(RGBOrigin a) {

  RGB res;
  res.R = a.R * SCALE;
  res.G = a.G * SCALE;
  res.B = a.B * SCALE;
  return res;
}

inline RGBAOrigin tori(RGBA res) {

  RGBAOrigin ores;
  ores.R = (unsigned char)clamp(res.R * 255.0f + 0.5f, 0.0f, 255.0f);
  ores.G = (unsigned char)clamp(res.G * 255.0f + 0.5f, 0.0f, 255.0f);
  ores.B = (unsigned char)clamp(res.B * 255.0f + 0.5f, 0.0f, 255.0f);
  ores.A = (unsigned char)clamp(res.A * 255.0f + 0.5f, 0.0f, 255.0f);
  return ores;
}

inline float sum(RGB a) { return a.R + a.G + a.B; }

inline float vecdot(RGB a, RGB b) { return a.R * b.R + a.G * b.G + a.B * b.B; }

__kernel void arrmix(__global const RGBOrigin *p0, __global const RGBOrigin *p1,
                     __global RGBAOrigin *pr, float k) {
  RGB pixela, pixelb, pd, tv;
  RGBA res;
  float a, nb;
  int gid = get_global_id(0);
  pixela = fori(p0[gid]);
  pixelb = fori(p1[gid]);
  pd = vecaddn(vecsub(pixelb, pixela), 1);
  a = k * k * 3 + 3;
  nb = (a - 2) * sum(pd);
  tv = vecadd(vecaddn(vecadd(pixela, pixelb), -1), vecmn(pd, k));
  res.A = max(
      ((nb + sqrt(max(nb * nb - 8 * a * k * vecdot(pd, tv), 0.0f))) / 2 / a),
      0.00000001f);
  tv = vecaddn(vecmn(tv, 0.5 / res.A), (1 - k) / 2);
  res.R = tv.R;
  res.G = tv.G;
  res.B = tv.B;
  pr[gid] = tori(res);
}
