#include "craft_ing.h"

InventoryElement * craft_ing(int product_id, InventoryElement * el)
{
    InventoryElement * crafted=nullptr;

        switch (product_id)
        {
            case ING_AXE_BLADE:
            case ING_AXE_HANDLE:
            case ING_WALL:
                switch(product_id)
                {
                    case ING_AXE_BLADE: crafted  = new AxeBlade(el); break;
                    case ING_AXE_HANDLE: crafted = new AxeHandle(el); break;
                    case ING_WALL: crafted = new Wall(el); break;
                }

                if (crafted->craft())
                {
                    crafted->show();
                    return crafted;
                }
                else
                {
                    delete crafted;
                    return nullptr;
                }
               break;
        }
        return nullptr;
}
