use crate::core;
use serde::{Deserialize, Serialize};

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub enum ItemLocationLol {
    Chunk { x: i32, y: i32 },
    Player { id: usize },
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub struct InventoryElementData {
    pub uid: usize,
    pub el_type: core::Class_id,
    pub location: ItemLocationLol,
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub struct ElementData {
    pub base: InventoryElementData,
    pub id: i32,
    pub sharpness: u32,
    pub smoothness: u32,
    pub mass: u32,
    pub length: u32,
    pub width: u32,
    pub height: u32,
    pub volume: u32,
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub struct IngredientData {
    pub base: InventoryElementData,
    pub id: core::Ingredient_id,
    pub quality: i32,
    pub resilience: i32,
    pub usage: i32,
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub struct ProductData {
    pub base: InventoryElementData,
    pub id: core::Product_id,
    pub quality: i32,
    pub resilience: i32,
    pub usage: i32,
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub struct PlantData {
    pub base: InventoryElementData,
    pub id: core::plant_types,
    pub phase: core::Plant_phase,
    pub grown: bool,
}

#[repr(C)]
#[derive(Serialize, Deserialize)]
pub enum ObjectData {
    InvElement { data: InventoryElementData },
    Element { data: ElementData },
    Ingredient { data: IngredientData },
    Product { data: ProductData },
    Plant { data: PlantData },
    Animal { data: InventoryElementData },
}

#[no_mangle]
extern "C" fn foo() -> ObjectData {
    ObjectData::InvElement {
        data: InventoryElementData {
            uid: 123,
            el_type: 1,
            location: ItemLocationLol::Player { id: 1 },
        },
    }
}
