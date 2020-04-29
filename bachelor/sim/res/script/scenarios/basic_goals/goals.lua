--This file keeps track of all possible goals. Not all agents may necessarily use all goals.

goals = {
	eat = {
    name = "eat",
    age = 0,
    tags = ETag.Food,
    weight_function = function()
        return age*age
    end
	,
    change_over_time = function(dt)
        return ((age+dt)*(age+dt)) - (age*age)
    end
	,
    get_discontentment = function(value)
        return value * value
    end
    }
}






