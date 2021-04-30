//
// Created by Will Zhang on 2021/1/14.
//

#ifndef PITWINS_STATUSPUBLISHER_H
#define PITWINS_STATUSPUBLISHER_H


#include "Publisher.h"

class StatusPublisher : public Publisher {
public:
    explicit StatusPublisher(socket_t *publisher) : Publisher(publisher) {
    }

    void start() override {

    }
};


#endif //PITWINS_STATUSPUBLISHER_H
