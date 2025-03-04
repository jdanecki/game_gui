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
                    sharpness: element.sharpness,
                    smoothness: element.smoothness,
                    mass: element.mass,
                    length: element.length,
                    width: element.width,
                    height: element.height,
                    volume: element.volume,
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
                    quality: ingredient.quality,
                    resilience: ingredient.resilience,
                    usage: ingredient.usage,
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
                    quality: product.quality,
                    resilience: product.resilience,
                    usage: product.usage,
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
        InventoryElementData {
            uid: el.uid,
            el_type: el.c_id,
            location: ItemLocationLol::Chunk {
                x: el.location.data.chunk.x,
                y: el.location.data.chunk.y,
            },
        }
    }
}
