/*
 * semrelay.c
 *
 *  Created on: 2013-08-19
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <semaphore.h>

#include "semrelay.h"
#include "statistics.h"
#include "multilock.h"
#include "utils.h"

void *semrelay_worker(void *ptr) {
    unsigned long i, j;
    struct experiment *exp = ptr;

    for (i = 0; i < exp->outer; i++) {
        // TODO: attendre notre tour
        sem_wait(&(exp->lock[i]));
        for (j = 0; j < exp->inner; j++) {
            unsigned long idx = (i * exp->inner) + j;
            statistics_add_sample(exp->data, (double) idx);
        }
        // TODO: signaler le travailleur suivant
        sem_post(&(exp->lock[(i+1) % exp->outer]));
    }
    return NULL;
}

void semrelay_init(struct experiment *exp) {
    int i;

    exp->data = make_statistics();
    // TODO: allocation d'un tableau de sémaphores sem_t dans exp->lock
    exp->lock = malloc (sizeof(sem_t[exp->outer]));
    // TODO: initialisation des sémaphores
    sem_init( &(exp->lock[0]), 0, 1);
    for (i = 1; i < exp->outer; i++)
    {
        sem_init( &(exp->lock[i]), 0, 0);
    }

}

void semrelay_done(struct experiment *exp) {
    int i;

    // copie finale dans exp->stats
    statistics_copy(exp->stats, exp->data);
    free(exp->data);

    // TODO: destruction du verrou
    for (i = 0; i < exp->outer; i++)
    {
        sem_destroy(&(exp->lock[i]));
    }
    // TODO: liberation de la memoire du verrou
    free(exp->lock);
}

