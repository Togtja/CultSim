peronality.cannibal = {
    name = "Cannibal", -- Personality trait
    desc = "Increases the chances to attack the same species, and eat them", -- A description of what it does (Shows up as tool tip)

    affected_area = {
        -- Increases the chances of you attacking
        attack = { -- name, just used for cpp
            type = Action, -- Attacking is an action
            what = "Kill Species", -- What Action/Need
            weight_multi = 1.2, -- By How much it changes the weigthing of wanting to to that Action/Strategy
        }
        -- Heavely increasing your want to consume meat from the same species
        consume_species = {
            type = Action, 
            what = "Consume Other",
            weight_multi = 1.5,
        }
        -- Increases the change of wanting to eat yourself in emergencies
        consume_self = {
            type = Action,
            what = "Consume Self",
            weight_multi = 1.2
        }

        fast_digestion = {
            type = Need,
            what = "Food",
            weight_multi = 1.1,
            decay_multi = 1.2
        }
    }
    
}