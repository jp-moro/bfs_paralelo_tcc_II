#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Created by Rafael S. Guimaraes and Diego Cardoso
# NERDS - Nucleo de Estudos em Redes Definidas por Software
#
KERNEL_MURMUR3 = """
__kernel void murmur(__global int* A, __global int* B, int len, int seed)
{
 int h;
 int id;
 int *key;
 int i;
 int k;
 int* key_x4;
 int idCopy;

 h = seed;
 id = get_global_id(0);
 idCopy = id;
 key = A[id];
  if (len > 3) {
    key_x4 = key;
    i = len >> 2;
    do {
      k = A[idCopy];
      idCopy++;
      k *= 0xcc9e2d51;
      k = (k << 15) | (k >> 17);
      k *= 0x1b873593;
      h ^= k;
      h = (h << 13) | (h >> 19);
      h = (h * 5) + 0xe6546b64;
    } while (--i);\
    d
    key = (const int*) key_x4;
  }
  if (len & 3) {
    int i = len & 3;
    int k = 0;
    key = &key[i - 1];
    do {
      k <<= 8;
      k |= *key--;
    } while (--i);
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    h ^= k;
  }
  h ^= len;
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;
  B[id] = h % 32;
}
"""
