clear all; close all; clc

% einheitsvektoren um 120° versetzt
e1 = [1; 0;];
e2 = [cosd(120); sind(120)];
e3 = [cosd(-120); sind(-120)];

% hilfsvariablen zum zeichnen eines kreises
t = linspace(0, 2*pi);
xx = cos(t);
yy = sin(t);

angles = [];
ph2 = [];

my1 = [];
my2 = [];
my3 = [];

variant = 2;

% zeitvektor für die pwm
x = linspace(0, 1, 1001);


filename = 'popo.gif';


for phi = 0:5:359
    % erzeugt einen rotierenden zeiger in der alpha/beta ebene
    v_alpha_beta = [cosd(phi); sind(phi)];

    % inverse clarke transformation - berechnet die 3phasen grössen
    v_abc = 2/3 * [1 0; -1/2 sqrt(3)/2; -1/2 -sqrt(3)/2] * v_alpha_beta;
    
    % ergibt einen vektor mit allen winkeln
    angles = [angles; [phi v_abc']];

    % zeitsignale für die pwm initialisiert bei 0
    y1 = zeros(size(x));
    y2 = y1;
    y3 = y1;

    if variant == 2
      
        beta = sind(phi);
        alpha = cosd(phi);
        
        % inverse clarke transformation
        u = alpha;
        v = -.5*alpha + sqrt(3)/2*beta;
        w = -.5*alpha - sqrt(3)/2*beta;
        
        % Mittelwert des Min und des Max bilden
        mid = mean([min([u v w]) max([u v w])]);
        
        % von -1 1 auf 0 1 bringen, das geht mal Timer Reload in das 
        % Compare register des Timers
        t1 = (u - mid)*.577 + .5;
        t2 = (v - mid)*.577 + .5;
        t3 = (w - mid)*.577 + .5;
        
        % umwandlung auf deine Centeraligned darstellung.
        y1(x < t1) = 1;
        y2(x < t2) = 1;
        y3(x < t3) = 1;
        

    elseif variant == 0
       
        if phi >= 0 && phi < 60
            % sector 1
            % c 0 0 0 1 | 1 0 0 0
            % b 0 0 1 1 | 1 1 0 0
            % a 0 1 1 1 | 1 1 1 0

            t1 = sind(60 - phi);
            t2 = sind(phi);
            tt = (1 - t1 - t2);

            y3(x > 0.5-tt/4) = 1;
            y3(x > 0.5+tt/4) = 0;

            y2(x > 0.5-tt/4-t2/2) = 1;
            y2(x > 0.5+tt/4+t2/2) = 0;

            y1(x > 0.5-tt/4-t2/2-t1/2) = 1;
            y1(x > 0.5+tt/4+t2/2+t1/2) = 0; 
        elseif phi >= 60 && phi < 120
            % sector 2
            % c 1 0 0 0 0 0 0 1  |  1 0 0 0 0 0 0 1
            % b 1 1 1 0 0 1 1 1  |  1 1 1 0 0 1 1 1
            % a 1 1 0 0 0 0 1 1  |  1 1 0 0 0 0 1 1

            t1 = sind(60 - (phi-60));
            t2 = sind(phi-60);
            tt = (1 - t1 - t2);

            y3(x > 0.5-tt/4) = 1;
            y3(x > 0.5+tt/4) = 0;

            y2(x > 0.5-tt/4-t1/2-t2/2) = 1;
            y2(x > 0.5+tt/4+t1/2+t2/2) = 0;

            y1(x > 0.5-tt/4-t1/2) = 1;
            y1(x > 0.5+tt/4+t1/2) = 0;
        elseif phi >= 120 && phi < 180
            % sector 3
            % c 0 0 1 1 | 1 1 0 0
            % b 0 1 1 1 | 1 1 1 0
            % a 0 0 0 1 | 1 0 0 0

            t1 = sind(60 - (phi-120));
            t2 = sind(phi-120);
            tt = (1 - t1 - t2);

            y3(x > 0.5-tt/4-t2/2) = 1;
            y3(x > 0.5+tt/4+t2/2) = 0;

            y2(x > 0.5-tt/4-t2/2-t1/2) = 1;
            y2(x > 0.5+tt/4+t2/2+t1/2) = 0;

            y1(x > 0.5-tt/4) = 1;
            y1(x > 0.5+tt/4) = 0;
        elseif phi >= 180 && phi < 240
            % sector 4
            % c 1 1 1 0 0 1 1 1  |  1 1 1 0 0 1 1 1
            % b 1 1 0 0 0 0 1 1  |  1 1 0 0 0 0 1 1
            % a 1 0 0 0 0 0 0 1  |  1 0 0 0 0 0 0 1

            t1 = sind(60 - (phi-180));
            t2 = sind(phi-180);
            tt = (1 - t1 - t2);

            y3(x > 0.5-tt/4-t1/2-t2/2) = 1;
            y3(x > 0.5+tt/4+t1/2+t2/2) = 0;

            y2(x > 0.5-tt/4-t1/2) = 1;
            y2(x > 0.5+tt/4+t1/2) = 0;

            y1(x > 0.5-tt/4) = 1;
            y1(x > 0.5+tt/4) = 0;
        elseif phi >= 240 && phi < 300
            % sector 5
            % c 0 1 1 1 | 1 1 1 0
            % b 0 0 0 1 | 1 0 0 0
            % a 0 0 1 1 | 1 1 0 0

            t1 = sind(60 - (phi-240));
            t2 = sind(phi-240);
            tt = (1 - t1 - t2);

            y3(x > 0.5-tt/4-t2/2-t1/2) = 1;
            y3(x > 0.5+tt/4+t2/2+t1/2) = 0;

            y2(x > 0.5-tt/4) = 1;
            y2(x > 0.5+tt/4) = 0;

            y1(x > 0.5-tt/4-t2/2) = 1;
            y1(x > 0.5+tt/4+t2/2) = 0;
        elseif phi >= 300 && phi < 360
            % sector 6
            % c 1 1 0 0 0 1 1  |  1 1 0 0 0 1 1
            % b 1 0 0 0 0 0 1  |  1 0 0 0 0 0 1
            % a 1 1 1 0 1 1 1  |  1 1 1 0 1 1 1

            t1 = sind(60 - (phi-300));
            t2 = sind(phi-300);
            tt = (1 - t1 - t2);

            y3(x > 0.5-tt/4-t1/2) = 1;
            y3(x > 0.5+tt/4+t1/2) = 0;

            y2(x > 0.5-tt/4) = 1;
            y2(x > 0.5+tt/4) = 0;

            y1(x > 0.5-tt/4-t1/2-t2/2) = 1;
            y1(x > 0.5+tt/4+t1/2+t2/2) = 0;
        end
    else
        if phi >= 0 && phi < 60
            % sector 1
            % c 0 0 0 0 0 0 0
            % b 0 0 1 0 1 0 0
            % a 0 1 1 0 1 1 0

            t1 = sind(60 - phi);
            t2 = sind(phi);
            tt = (1 - t1 - t2);

            y2(x > 0.5-t2/2) = 1;
            y2(x > 0.5+t2/2) = 0;

            y1(x > 0.5-t1/2-t2/2) = 1;
            y1(x > 0.5+t1/2+t2/2) = 0; 
        elseif phi >= 60 && phi < 120
            % sector 2
            % c 0 0 0 0 0 0 0
            % b 0 1 1 0 1 1 0
            % a 0 1 0 0 0 1 0

            t1 = sind(60 - (phi-60));
            t2 = sind(phi-60);
            tt = (1 - t1 - t2);

            y2(x > 0.5-t1/2-t2/2) = 1;
            y2(x > 0.5+t1/2+t2/2) = 0;

            y1(x > 0.5-t1/2) = 1;
            y1(x > 0.5+t1/2) = 0;
        elseif phi >= 120 && phi < 180
            % sector 3
            % c 0 0 1 0 1 0 0
            % b 0 1 1 0 1 1 0
            % a 0 0 0 0 0 0 0

            t1 = sind(60 - (phi-120));
            t2 = sind(phi-120);
            tt = (1 - t1 - t2);

            y3(x > 0.5-t2/2) = 1;
            y3(x > 0.5+t2/2) = 0;

            y2(x > 0.5-t1/2-t2/2) = 1;
            y2(x > 0.5+t1/2+t2/2) = 0;
        elseif phi >= 180 && phi < 240
            % sector 4
            % c 0 1 1 0 1 1 0
            % b 0 1 0 0 0 1 0
            % a 0 0 0 0 0 0 0

            t1 = sind(60 - (phi-180));
            t2 = sind(phi-180);
            tt = (1 - t1 - t2);

            y3(x > 0.5-t1/2-t2/2) = 1;
            y3(x > 0.5+t1/2+t2/2) = 0;

            y2(x > 0.5-t1/2) = 1;
            y2(x > 0.5+t1/2) = 0;
        elseif phi >= 240 && phi < 300
            % sector 5
            % c 0 1 1 0 1 1 0
            % b 0 0 0 0 0 0 0
            % a 0 0 1 0 1 0 0

            t1 = sind(60 - (phi-240));
            t2 = sind(phi-240);
            tt = (1 - t1 - t2);

            y3(x > 0.5-t1/2-t2/2) = 1;
            y3(x > 0.5+t1/2+t2/2) = 0;

            y1(x > 0.5-t2/2) = 1;
            y1(x > 0.5+t2/2) = 0;
        elseif phi >= 300 && phi < 360
            % sector 6
            % c 0 1 0 0 0 1 0
            % b 0 0 0 0 0 0 0
            % a 0 1 1 0 1 1 0

            t1 = sind(60 - (phi-300));
            t2 = sind(phi-300);
            tt = (1 - t1 - t2);

            y3(x > 0.5-t1/2) = 1;
            y3(x > 0.5+t1/2) = 0;

            y1(x > 0.5-t1/2-t2/2) = 1;
            y1(x > 0.5+t1/2+t2/2) = 0;
        end
    end


    % bilden der mittelwerte der pwm
    my1 = [my1; mean(y1)];
    my2 = [my2; mean(y2)];
    my3 = [my3; mean(y3)];

    % v1 = v_abc(1)*e1;
    % v2 = v_abc(2)*e2;
    % v3 = v_abc(3)*e3;

    % berechnen der länge der jeweiligen vektoren
    v1 = 2/sqrt(3)*mean(y1)*e1;
    v2 = 2/sqrt(3)*mean(y2)*e2;
    v3 = 2/sqrt(3)*mean(y3)*e3;

    
    figure(1);
    subplot(321);
    stairs(x,y1,'r');
    ylim([-0.1 1.1]);
    title('PWM a');

    subplot(323);
    stairs(x,y2,'g');
    ylim([-0.1 1.1]);
    title('PWM b');

    subplot(325);
    stairs(x,y3,'m');
    ylim([-0.1 1.1]);
    title('PWM c');

    subplot(322);
    plot(angles(:,1),my1,'r', angles(:,1),my2,'g', angles(:,1),my3,'m', angles(:,1),(my1+my2+my3)./3, 'k');
    xlim([0 359]);
    ylim([0 1]);
    title('avg. output voltage');

    subplot(324);
    plot(angles(:,1),my1-my2,angles(:,1),my2-my3,angles(:,1),my3-my1, ...
        angles(:,1),angles(:,2),'--',angles(:,1),angles(:,3),'--',angles(:,1),angles(:,4),'--');
    xlim([0 359]);
    ylim([-1 1]);
    title('phase to phase voltage');

    subplot(326);
    plot(xx,yy);
    daspect([1 1 1]);
    xlim([-1.2 1.2]);
    ylim([-1.2 1.2]);
    alphabeta = line([0 v_alpha_beta(1)], [0 v_alpha_beta(2)]);
    vector1 = line([0 v1(1)], [0 v1(2)], 'Color',[1 0 0]);
    vector2 = line([v1(1) v1(1)+v2(1)], [v1(2) v1(2)+v2(2)], 'Color', [0 1 0]);
    vector3 = line([v1(1)+v2(1) v1(1)+v2(1)+v3(1)], [v1(2)+v2(2) v1(2)+v2(2)+v3(2)], 'Color', [1 0 1]);
    title('vectors');

    drawnow

    frm = getframe(1);
    im = frame2im(frm);
    [imind,cm] = rgb2ind(im,256);
    if phi==0
        imwrite(imind,cm,filename,'gif','Loopcount', inf,'DelayTime',0.1);
    else
        imwrite(imind,cm,filename,'gif','WriteMode','append','DelayTime',0.1);
    end

    delete(alphabeta);
    delete(vector1);
    delete(vector2);
    delete(vector3);
end
