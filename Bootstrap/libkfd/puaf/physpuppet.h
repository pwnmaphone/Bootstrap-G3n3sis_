/*
 * Copyright (c) 2023 Félix Poulin-Bélanger. All rights reserved.
 */

#ifndef physpuppet_h
#define physpuppet_h

#include "../../libkfd.h"
#include "dynamic_info.h"
#include "static_info.h"
#include "../common.h"

void physpuppet_init(struct kfd* kfd);

void physpuppet_run(struct kfd* kfd);

void physpuppet_cleanup(struct kfd* kfd);

void physpuppet_free(struct kfd* kfd);

#endif /* physpuppet_h */
