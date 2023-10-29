angeldiffdata = readtable("Motor_Kalibrierung_Daten_Continues_no_jump_adjusted_different_start.txt");

n = size(angeldiffdata,1);
n = 120;

Measured_Angel = zeros(n, 1);
Reference_Angle = zeros(n, 1);
Angle_Error = zeros(n, 1);

angle_Measurements = table2array(angeldiffdata);
m = 0;

p =1;
while(p-1 < n)
    
    Measured_Angel(p) = angle_Measurements(p, 2);
    Reference_Angle(p) = angle_Measurements(p, 1);
    
    if(Measured_Angel(p) > 360)
        Measured_Angel(p) = 359.9; 
    end
    
    Angle_Error(p) = mod((Measured_Angel(p) - Reference_Angle(p)) + 180,360) - 180;

    p= p+1;
end



plot(Reference_Angle, Measured_Angel);
%title('Strom-Drehzahl-Kennlinien');
xlabel('Referenz Winkel');
ylabel('Gemessener Winkel');
ylim([-20 380]);
xlim([-20 380]);