-- Defines custom systems
systems = {
    be_stupid_system = cultlib.create_system("Be stupid", {
        initialize = function() log.info("Hi, I am the stupid system!") end,
        update = function(dt) end,
        deinitialize = function() log.info("Bye, I was the stupid system!") end
    })
}