entity = {
    PositionComponent = {
        position = Vec2:new(random:uniform(-scenario.bounds.x, scenario.bounds.x),
                            random:uniform(-scenario.bounds.y, scenario.bounds.y))
    },
    MovementComponent = {
        speed = 80.0
    },
    SpriteComponent = {
        color = Vec3:new(1.0, 1.0, 1.0),
        texture = "sprites/deer_c.png",
        normal = "sprites/deer_n.png",
        scale = 20
    },
    VisionComponent = {
        radius = 40.0,
        fov = 0 -- 0 means no FOV
    },
    TagComponent = {
        tags = ETag.Avoidable | ETag.Creature
    },
    NeedComponent = {
        -- Corresponds to entires in the needs.lua file
        required_needs = { needs.hunger, needs.thirst, needs.sleep},
        leisure_needs = { needs.stockpile, needs.reproduce, needs.steal }
    },
    ReproductionComponent = {
        sex = random:randint(0, 1),
        mean_offspring = 2,
        offspring_deviation = 1,
        average_gestation = 90,
        gestation_deviation = 5
    },
    StrategyComponent = {
        -- Corresponds to entries in the strategies.lua file
        strategies = {strategies.forage_for_food, strategies.steal_food, strategies.find_food, strategies.consume_from_backpack,strategies.drink_from_backpack, strategies.find_water, strategies.sleep_on_ground, strategies.find_mate, strategies.loot_food, strategies.loot_water}
    },
    HealthComponent = {
        health = 100.0,
        tickdown_rate = 1.0,
        vital_needs = ETag.Food | ETag.Drink | ETag.Sleep
    },
    InventoryComponent = {
        max_size = 5
    },
    MemoryComponent = {
        max_memories = 10,
        max_retention_time = 1000.0,
        allowed_memories = { ETag.Food | ETag.Consume | ETag.Location, ETag.Drink | ETag.Consume | ETag.Location }
    },
    AgeComponent = 
    {
        life_expectancy = 3000.0
    },
    NameComponent = 
    {
     entity_type = "Deer"
    },
    TraitComponent = {
        -- Other parts of lua code may add and remove a trait(s)
        -- However this is what all the "Deers" would start with
        start_traits = {{trait = traits.fast, chance = 0.7}, traits.slow_digestion},
        -- Obtainable traits
        attainable_traits = {traits.slow_digestion, traits.random_mutation, traits.fast}
    },
    RelationshipComponent = {},
    Sphere3DComponent = 
    {
        radius = 0.05
    }
}
