classdef PendulumApp < matlab.apps.AppBase
    properties (Access = private)
        UIFigure        matlab.ui.Figure
        Axes            matlab.ui.control.UIAxes
        PendulumRod     matlab.graphics.primitive.Line
        PendulumBall    matlab.graphics.primitive.Surface
        hgTransform     matlab.graphics.primitive.Transform
    end
    
    methods (Access = private)
        function createComponents(app)
            app.UIFigure = uifigure('Name', 'Pendulum Visualization');
            app.Axes = uiaxes(app.UIFigure);
            
            [X, Y, Z] = cylinder(0.02, 20);
            app.PendulumRod = surf(app.Axes, X, Y, Z, 'FaceColor', 'red');
            
            [X, Y, Z] = sphere(20);
            app.PendulumBall = surf(app.Axes, X*0.1, Y*0.1-1, Z*0.1, 'FaceColor', 'blue');
            
            app.hgTransform = hgtransform('Parent', app.Axes);
            set(app.PendulumRod, 'Parent', app.hgTransform);
            set(app.PendulumBall, 'Parent', app.hgTransform);
        end
        
        function updatePendulum(app, angle)
            T = makehgtform('zrotate', angle);
            set(app.hgTransform, 'Matrix', T);
        end
    end
end