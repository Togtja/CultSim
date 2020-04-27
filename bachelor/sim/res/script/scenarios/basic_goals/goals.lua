--This file keeps track of all possible goals. Not all agents may necessarily use all goals.

goals = {
	eat = {
    name = "eat",
    weight = 0,
    age = 0,
    tags = ETag.Food,
    weight_function = function()
    end
	,
    change_over_time = function(dt)
    end
	,
    get_discontentment = function(value)
    end
    }
}