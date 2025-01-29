use crate::core;
use crate::types::*;

pub fn convert_to_data(el: &core::InventoryElement) -> ObjectData {
    match el.c_id {
        core::Class_id_Class_Element => {
            let element =
                unsafe { &*(el as *const core::InventoryElement as *const core::Element) };
            ObjectData::Element {
                data: ElementData {
                    base: convert_inv_el(el),
                    id: unsafe { (*element.base).id },
                    sharpness: (*element.sharpness).value,
                    smoothness: (*element.smoothness).value,
                    mass: (*element.mass).value,
                    length: (*element.length).value,
                    width: (*element.width).value,
                    height: (*element.height).value,
                    volume: (*element.volume).value,
                },
            }
        }
        core::Class_id_Class_Ingredient => {
            let ingredient =
                unsafe { &*(el as *const core::InventoryElement as *const core::Ingredient) };
            ObjectData::Ingredient {
                data: IngredientData {
                    base: convert_inv_el(el),
                    id: ingredient.id,
                    quality: (*ingredient.quality).value,
                    resilience: (*ingredient.resilience).value,
                    usage: (*ingredient.usage).value,
                },
            }
        }
        core::Class_id_Class_Product => {
            let product =
                unsafe { &*(el as *const core::InventoryElement as *const core::Product) };
            ObjectData::Product {
                data: ProductData {
                    base: convert_inv_el(el),
                    id: product.id,
                    quality: (*product.quality).value,
                    resilience: (*product.resilience).value,
                    usage: (*product.usage).value,
                },
            }
        }
        core::Class_id_Class_Plant => {
            let plant = unsafe { &*(el as *const core::InventoryElement as *const core::Plant) };
            ObjectData::Plant {
                data: PlantData {
                    base: convert_inv_el(el),
                    id: plant.type_,
                    phase: plant.phase,
                    grown: plant.grown,
                },
            }
        }
        core::Class_id_Class_Animal => {
            // let animal = unsafe { &*(el as *const core::InventoryElement as *const core::Animal) };
            ObjectData::Animal {
                data: convert_inv_el(el),
            }
        }
        _ => panic!(),
    }
}

fn convert_inv_el(el: &core::InventoryElement) -> InventoryElementData {
    unsafe {
        let location = if el.location.type_ == core::ItemLocationType_LOCATION_CHUNK {
            ItemLocationLol::Chunk {
                map_x: el.location.data.chunk.map_x,
                map_y: el.location.data.chunk.map_y,
                x: el.location.data.chunk.x,
                y: el.location.data.chunk.y,
            }
        } else {
            ItemLocationLol::Player {
                id: el.location.data.player.id as usize,
            }
        };
        InventoryElementData {
            uid: el.uid,
            el_type: el.c_id,
            location,
        }
    }
}

pub fn convert_item_location(location: &core::ItemLocation) -> ItemLocationLol {
    unsafe {
        if location.type_ == core::ItemLocationType_LOCATION_CHUNK {
            ItemLocationLol::Chunk {
                map_x: location.data.chunk.map_x,
                map_y: location.data.chunk.map_y,
                x: location.data.chunk.x,
                y: location.data.chunk.y,
            }
        } else {
            ItemLocationLol::Player {
                id: location.data.player.id as usize,
            }
        }
    }
}
