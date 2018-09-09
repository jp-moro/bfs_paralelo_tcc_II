#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Created by Rafael S. Guimaraes and Diego Cardoso
# NERDS - Nucleo de Estudos em Redes Definidas por Software
#
"""Active Kernels."""

KERNEL_MATCH = """
__kernel void match(__global unsigned int* ips, __global int* pos, unsigned int key)
{
    int id;
    id = get_global_id(0);
    // printf("%d %d %d %d\\n", id, key, ips[id], ips[id] ^ key);
    if(!(ips[id] ^ key))
    {
        pos[0] = id;
    }
}
"""
