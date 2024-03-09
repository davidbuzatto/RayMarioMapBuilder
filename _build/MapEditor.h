#pragma once
#include "Drawable.h"

class MapEditor : public virtual Drawable {

public:

    MapEditor();
    ~MapEditor();

    static void inputAndUpdate();
    virtual void draw() const;

};

