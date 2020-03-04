-- Lua configuration file for the application

action_key_bindings = {
    [EKeyContext.ScenarioScene] = {
        ["q"] = EAction.ZoomIn,
        ["e"] = EAction.ZoomOut,
        ["w"] = EAction.MoveUp,
        ["a"] = EAction.MoveLeft,
        ["s"] = EAction.MoveDown,
        ["d"] = EAction.MoveRight
    }
}
action_mouse_bindings = {
    [EKeyContext.ScenarioScene] = {
        [EMouse.WheelUp] = EAction.ZoomIn,
        [EMouse.WheelDown] = EAction.ZoomOut
    }
}
