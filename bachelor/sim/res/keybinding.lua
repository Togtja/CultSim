-- Lua configuration file for the application

action_key_bindings = {
    [EKeyContext.ScenarioScene] = {
        ["q"] = EAction.ZoomIn,
        ["e"] = EAction.ZoomOut,
        ["w"] = EAction.MoveUp,
        ["a"] = EAction.MoveLeft,
        ["s"] = EAction.MoveDown,
        ["d"] = EAction.MoveRight,
        ["f"] = EAction.FollowEntity,
        ["p"] = EAction.PauseMenu,
        ["Escape"] = EAction.PauseMenu,
        ["."] = EAction.SpeedUp,
        [","] = EAction.SpeedDown,
        ["Space"] = EAction.Pause
    }
}
action_mouse_bindings = {
    [EKeyContext.ScenarioScene] = {
        [EMouse.BtnWheelUp] = EAction.ZoomIn,
        [EMouse.BtnWheelDown] = EAction.ZoomOut,
        [EMouse.BtnLeft] = EAction.SelectEntity
    }
}
