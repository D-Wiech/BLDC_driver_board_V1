%% sensor data definition
%angle_input = [0:11.25:348.75];
angle_input = Reference_Angle';
%sensor_data = [266.31 278.61 290.39 301.99 312.45 323.00 332.75 342.69 352.79 3.16 14.24 26.02 38.94 52.91 67.15 82.18 97.12 111.45 124.98 137.46 148.62 158.82 167.96 176.48 184.48 192.92 201.27 210.50 220.43 230.98 242.31 254.36];
sensor_data = Measured_Angel';

%% Check rising reference angle
if any(angle_input<0) || any(angle_input>360) || any(diff(angle_input)<=0)
error('reference angle must be monotonuously rising between 0 and 360');
end
%% Check correct sensor angle range
if any(sensor_data<0) || any(sensor_data>360)
error('sensor angle must be between 0 and 360');
end
%% pre-processing
sensor_data_2 = sensor_data(:);
% check if rising/falling continuously with maximum one overflow
if any(diff(sensor_data_2) == 0)
error('sensor data must be monotonously increasing or decreasing')
elseif sum(diff(sensor_data_2) < 0 ) <= 1
% rising angle data with zero or one overflows, overflow will be corrected
sensor_data_2 = sensor_data_2(:) + 360*cumsum([false; diff(sensor_data_2(:)) < 0 ]);
elseif sum(diff(sensor_data_2) > 0 ) <= 1
% falling angle data with zero or one overflows, overflow will be corrected
sensor_data_2 = sensor_data_2(:) - 360*cumsum([false; diff(sensor_data_2(:)) > 0 ]);
else
error('only one data overflow permitted')
end
% invert angle direction, if needed
if all(diff(sensor_data_2) < 0 )
disp('inverting angle direction...')
sensor_data_2 = -sensor_data_2;
raw_direction = -1;
elseif all(diff(sensor_data_2) > 0 )
raw_direction = +1;
else
error('nonmonotonic angle changes detected')
end
% correctly wrap around sensor data
rollovercorrection = round((mean(sensor_data_2) - 180)/360) * 360;
sensor_data_2 = sensor_data_2 - rollovercorrection;
%extend sensor data
sensor_data_ext = [sensor_data_2(:); sensor_data_2(:)+360; ...
sensor_data_2(:)+720];
%extend input data
angle_input_ext = [angle_input(:); angle_input(:)+360; ...
angle_input(:)+720];
%% plot magnet measurements after preprocessing finished
figure;plot([angle_input(:)],[sensor_data_2(:)],'o-');
xlabel('Encoder angle [deg]');
ylabel('Output angle [deg]');
grid on;
xlim([0 360]);
if (raw_direction == +1)
title('Output field direction over encoder direction');
else
title('Output field direction over encoder direction after direction inversion');
end
%% use spline to move the data from an ordered input grid
% onto an ordered output grid:
ordered_output_grid = 0:(360/4096):(360-360/4096);
intermediategrid = ordered_output_grid + 360;
projection = spline(sensor_data_ext, angle_input_ext, ...
intermediategrid);
% calculate the required correction of the data:
correction_curve = projection - intermediategrid;
correction_curve = correction_curve(:);
%% piecewise linear approximation of the correction curve
lin_sup_nodes = 32;
% repeat the correction table three times to avoid
% corner effects on correction calculation.
triple_correction_curve = repmat(correction_curve,3,1);
triple_correction_curve(end+1) = triple_correction_curve(1);
% do the same with the angle input
triple_output_grid = 0:(360/4096):(3*360);
% calculate support points
XI_lin_triple = linspace(0,3*360,lin_sup_nodes*3+1);
YI_lin_triple = lsq_lut_piecewise( triple_output_grid(:),triple_correction_curve, XI_lin_triple );
% use only the central points to calculate the correction:
YI_lin = YI_lin_triple(lin_sup_nodes+1 : 2*lin_sup_nodes+1);
XI_lin = linspace(0,360,lin_sup_nodes+1);
%% calculate AAS330x1 parameters
% "zal": zero-after-linearization is always zero
disp(['*** AAS330x1 "zal" parameter in EEPROM should be set to 1']);
% "eli": enable linearization is always one
disp(['*** AAS330x1 "eli" parameter in EEPROM should be set to 1']);
% "ro": Die rotation should be enabled if direction of sensor angles was decreasing.
if (raw_direction == -1)
sensor_EEPROM_val_ro = 1;
else
sensor_EEPROM_val_ro = 0;
end
disp(['*** AAS330x1 "ro" parameter in EEPROM should be set to ' num2str(sensor_EEPROM_val_ro)]);
% "zero_offset": Offset should set as average of min and max offset.
% This will make best use of the range of +/-45 degrees that the linearization parameters have.
zero_offset = mean([min(YI_lin) max(YI_lin)]);
% sensor offset is subtracted from angle data. If sensor direction is inverted, this takes place before angle inversion,
% so that the sensor offset sign must be inverted in that case
sensor_EEPROM_val_zero_offset = uint16(mod(round(-zero_offset/360*4096),4096));
disp(['*** AAS330x1 "zero_offset" parameter in EEPROM should be set to ' num2str(sensor_EEPROM_val_zero_offset)]);
% "ls": Linearization scale must be set depending on the maximum parameter values.
linearization_range_small = 22.5 * (2047/2048);
linearization_range_large = 45.0 * (2047/2048);
if round(max(abs(YI_lin - zero_offset))) < linearization_range_small
sensor_EEPROM_val_ls = uint16(0);
elseif round(max(abs(YI_lin - zero_offset))) < linearization_range_large
sensor_EEPROM_val_ls = uint16(1);
else
error('Linearization parameters outside of +/-45° range; linearization not possible')
end
disp(['*** AAS330x1 "ls" parameter in EEPROM should be set to ' num2str(sensor_EEPROM_val_ls)]);
% "LIN_##" parameters
if (sensor_EEPROM_val_ls == 0) % small range of +/- 22.5°
sensor_EEPROM_val_LIN = int16(round((zero_offset-YI_lin(1:lin_sup_nodes))/22.5*2048));
else % larger range of +/- 45.0°
sensor_EEPROM_val_LIN = int16(round((zero_offset-YI_lin(1:lin_sup_nodes))/45.0*2048));
end
disp(['*** AAS330x1 "LIN_##" parameters in EEPROM should be set to the following values:']);
disp(num2str(sensor_EEPROM_val_LIN));
%% write csv table for Allegro AAS330x1 Samples Programmer
EEPtable = cell(38,2);
EEPtable(1,1:2) = {'EEPROM',''};
EEPtable(2,1:2) = {'zal',1};
EEPtable(3,1:2) = {'eli',1};
EEPtable(4,1:2) = {'ro',sensor_EEPROM_val_ro};
EEPtable(5,1:2) = {'zero_offset',sensor_EEPROM_val_zero_offset};
EEPtable(6,1:2) = {'ls',sensor_EEPROM_val_ls};
for i = 1:32
EEPtable{6+i,1} = num2str(['Linearization Error Segment ' num2str(i-1,'%02d')]);
EEPtable{6+i,2} = num2str(sensor_EEPROM_val_LIN(i));
end
cell2csv('EEP_table.csv',EEPtable);
%% perform linearization in Matlab
restored_linear_signal = mod(raw_direction*sensor_data(:) + ...
interp1(XI_lin,YI_lin,mod(raw_direction*sensor_data(:),360),'linear'),360);
%% plot the remaining error after linearization in Matlab
figure;
plot(angle_input(:),mod(180+restored_linear_signal(:)-angle_input(:),360)-180,'.-');
grid on;legend('Linear interpolation (32 segments)');
xlim([0 360]); ylim([-1 1]);xlabel('Measured angle [deg]');
ylabel('Expected error after linearization [deg]');
