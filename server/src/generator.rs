use std::collections::HashMap;
use std::rc::Rc;

use crate::core;
use crate::SEED;
use rand::prelude::*;

const REGIONS_NUM: u32 = 10;

pub fn generate() {
    unsafe {
        SEED = core::time(std::ptr::null_mut());
        println!("{}", SEED);
        core::srand(SEED as u32);
        println!("{}", core::rand());
        core::init_elements();
        //core::generator();
    }
    let terrains = create_terrains();
    let plants = create_plants(&terrains);
    let animals = create_animals(&plants);
    let mut regions = create_regions(&terrains, &plants, &animals);

    simulate(&mut regions);
    println!("{:#?}", regions[0]);
}

fn simulate(regions: &mut Vec<Region>) {
    for r in regions {
        for (plant, num) in r.active_plants.iter_mut() {
            *num += *num * plant.growth_speed as f32 * plant.possible_ground[&*r.terrain_type];
        }

        let occupied_space: f32 = r.active_plants.iter().map(|(k, v)| k.size as f32 * v).sum();
        let sun_amount = (r.size * core::CHUNK_SIZE * core::CHUNK_SIZE) as f32;
        if occupied_space > sun_amount {
            for (_, num) in r.active_plants.iter_mut() {
                *num = *num / (occupied_space / sun_amount);
            }
        }
        //        r.active_plants.iter()
        // old plants die
        //    lifespan
        // TODO some get eaten
        //    animals that live here eat
        // new plants grow depending on:
        //    num * ground_modifier * growth_speed_modifier
        //    capped at space avaible
    }
}

fn create_terrains() -> Vec<Rc<TerrainType>> {
    let mut terrains = Vec::<Rc<TerrainType>>::with_capacity(REGIONS_NUM as usize);
    for i in 0..REGIONS_NUM {
        terrains.push(Rc::new(TerrainType::new(i)));
    }
    terrains
}

fn create_regions(
    terrains: &Vec<Rc<TerrainType>>,
    plants: &Vec<Rc<PlantType>>,
    animals: &Vec<Rc<AnimalType>>,
) -> Vec<Region> {
    let mut rng = rand::rng();
    let mut regions = vec![];
    for i in 0..REGIONS_NUM as usize {
        let r = Region::new(
            Rc::clone(&terrains[i]),
            plants,
            animals,
            rng.random_range(0..core::WORLD_SIZE),
            rng.random_range(0..core::WORLD_SIZE),
        );
        regions.push(r);
    }

    for y in 0..core::WORLD_SIZE {
        for x in 0..core::WORLD_SIZE {
            let r = regions
                .iter_mut()
                .min_by_key(|r| r.coords.distance_squared(&Coords::new(x as i32, y as i32)))
                .unwrap();
            r.size = r.size + 1;
        }
    }
    regions
}

fn create_plants(terrains: &Vec<Rc<TerrainType>>) -> Vec<Rc<PlantType>> {
    let n = rand::random_range(10..20);
    let mut plants = Vec::<Rc<PlantType>>::with_capacity(n);
    for i in 0..n {
        plants.push(Rc::new(PlantType::new(i as u32, terrains)));
    }

    plants
}

fn create_animals(plants: &Vec<Rc<PlantType>>) -> Vec<Rc<AnimalType>> {
    let n = rand::random_range(5..15);
    let mut animals = Vec::<Rc<AnimalType>>::with_capacity(n);
    for i in 0..n {
        animals.push(Rc::new(AnimalType::new(i as u32, plants, &animals)));
    }

    animals
}

#[derive(Debug)]
struct Coords {
    pub x: i32,
    pub y: i32,
}

impl Coords {
    fn new(x: i32, y: i32) -> Coords {
        Coords { x, y }
    }
    fn distance_squared(&self, other: &Coords) -> i32 {
        (self.x - other.x) * (self.x - other.x) + (self.y - other.y) * (self.y - other.y)
    }
}

#[derive(Hash, PartialEq, Eq, Debug)]
struct TerrainType {
    id: u32,
}

impl TerrainType {
    fn new(id: u32) -> TerrainType {
        TerrainType { id }
    }
}

trait Food: std::fmt::Debug {}

#[derive(Debug)]
struct PlantType {
    pub id: u32,
    pub possible_ground: HashMap<Rc<TerrainType>, f32>,
    pub size: u32,
    //pub sun_required: u32,
    pub growth_speed: f32,
    // TODO
    // overcrowd immunity
}

impl PlantType {
    fn new(id: u32, terrains: &Vec<Rc<TerrainType>>) -> PlantType {
        let n = rand::random_range(1..terrains.len());
        let mut possible_ground = HashMap::new();
        possible_ground.insert(Rc::clone(terrains.choose(&mut rand::rng()).unwrap()), 1.0);
        for _ in 1..n {
            let choice = terrains.choose(&mut rand::rng()).unwrap();
            if !possible_ground.contains_key(&**choice) {
                possible_ground.insert(Rc::clone(choice), rand::random_range(0.1..1.2));
            }
        }
        PlantType {
            id,
            possible_ground,
            size: rand::random_range(1..3),
            growth_speed: rand::random_range(0.05..0.5),
        }
    }
}

impl PartialEq for PlantType {
    fn eq(&self, other: &Self) -> bool {
        self.id == other.id
    }
}

impl Eq for PlantType {}

impl std::hash::Hash for PlantType {
    fn hash<H: std::hash::Hasher>(&self, state: &mut H) {
        self.id.hash(state);
    }
}

impl Food for PlantType {}

#[derive(Debug)]
struct AnimalType {
    pub id: u32,
    pub possible_food: Vec<(Rc<dyn Food>, f32)>,
    pub required_food: f32,
}

impl Food for AnimalType {}

impl AnimalType {
    fn new(id: u32, plants: &Vec<Rc<PlantType>>, animals: &Vec<Rc<AnimalType>>) -> AnimalType {
        let food_num = rand::random_range(1..5);
        let mut possible_food: Vec<(Rc<dyn Food>, f32)> = Vec::with_capacity(food_num);
        if animals.len() > 0 {
            for _ in 0..food_num {
                let food = match rand::random_bool(0.5) {
                    true => Rc::clone(plants.choose(&mut rand::rng()).unwrap()) as Rc<dyn Food>,
                    false => Rc::clone(animals.choose(&mut rand::rng()).unwrap()) as Rc<dyn Food>,
                };
                possible_food.push((food, rand::random_range(0.1..1.1)));
            }
        } else {
            for _ in 0..food_num {
                let food = plants.choose(&mut rand::rng()).unwrap();
                possible_food.push((
                    Rc::clone(food) as Rc<dyn Food>,
                    rand::random_range(0.1..1.1),
                ));
            }
        }
        AnimalType {
            id,
            possible_food,
            required_food: rand::random_range(1.0..10.0),
        }
    }
}

impl PartialEq for AnimalType {
    fn eq(&self, other: &Self) -> bool {
        self.id == other.id
    }
}

impl Eq for AnimalType {}

impl std::hash::Hash for AnimalType {
    fn hash<H: std::hash::Hasher>(&self, state: &mut H) {
        self.id.hash(state);
    }
}

#[derive(Debug)]
struct Region {
    pub terrain_type: Rc<TerrainType>,
    //pub liquid_type: u32,
    pub size: u32,
    pub coords: Coords,
    pub active_plants: HashMap<Rc<PlantType>, f32>,
    pub active_animals: HashMap<Rc<AnimalType>, f32>,
}

impl<'a> Region {
    pub fn new(
        terrain_type: Rc<TerrainType>,
        plants: &Vec<Rc<PlantType>>,
        animals: &Vec<Rc<AnimalType>>,
        x: u32,
        y: u32,
    ) -> Region {
        let n = rand::random_range(5..10);
        let mut active_plants = HashMap::with_capacity(n);
        let choices = plants.choose_multiple(&mut rand::rng(), n);
        for plant in choices {
            if plant.possible_ground.keys().any(|t| *t == terrain_type) {
                active_plants.insert(Rc::clone(plant), 100.0);
            }
        }
        let n = rand::random_range(5..10);
        let mut active_animals = HashMap::with_capacity(n);
        let choices = animals.choose_multiple(&mut rand::rng(), 10);
        for animal in choices {
            active_animals.insert(Rc::clone(animal), 5.0);
        }
        Region {
            terrain_type,
            //liquid_type: 0,
            size: 0,
            coords: Coords::new(x as i32, y as i32),
            active_plants,
            active_animals,
        }
    }
}
