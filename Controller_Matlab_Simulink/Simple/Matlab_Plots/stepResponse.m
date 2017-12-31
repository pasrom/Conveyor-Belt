%%
% file for printing the step response
clc;
close all;
% don't show the plots in window, save only to file
set(0,'DefaultFigureVisible','on')
% everything in latex style :)
set(groot,'defaulttextinterpreter','latex'); %alles latex :)
%set(groot,'defaultLineLineWidth',1);
set(groot,'defaultFigureColor','w');
set(groot, 'defaultAxesTickLabelInterpreter','latex');
set(groot, 'defaultLegendInterpreter','latex');
set(0,'defaultTextInterpreter','latex'); %trying to set the default
set(0,'defaultlinelinewidth',1);

dataSimulink = load('stepResponse.mat');
dataSimulinkOld = load('stepResponseNew.mat');
dataMessung = load('messungA.mat');
dataNewStep = load('stepNew.mat');
%rampMessung = load('ramp_soll_ist.mat');
rampMessung = load('stepRampNew.mat');
rampMessung2 = load('stepRampNew2.mat');
time = dataMessung.messungA.Time;
velocity = dataMessung.messungA.Velocity;
velocityOld = dataMessung.messungA.VelocityOld;

figure(1);
p2(1) = plot(time,velocity,'DisplayName','Stepresponse gemessen alt');
hold on;
grid on;
p2(2) = plot(dataSimulink.ans(1,:),dataSimulink.ans(2,:),'DisplayName','Step');
p2(3) = plot(dataSimulink.ans(1,:),dataSimulink.ans(3,:),'DisplayName','Stepresponse Simulink');
p2(4) = plot(dataNewStep.stepnew.Zeit,dataNewStep.stepnew.istNew2re,'DisplayName','Stepresponse gemessen neu');
% Plot settling time point
%riseTimePlot(dataSimulink.ans(1,:),dataSimulink.ans(3,:));
riseTimePlot(time,velocity,1000,p2(1).Color,50);
riseTimePlot(dataNewStep.stepnew.Zeit,dataNewStep.stepnew.istNew2re,1000,p2(4).Color,-50);
ylabel({'Geschwindigkeit $v$ in RPM'});
xlabel({'Zeit $t$ in $s$'});
legend(p2,'Location','southoutside','Orientation','horizontal');
h=gcf;
set(h,'PaperOrientation','landscape');
set(h,'PaperUnits','normalized');
set(h,'PaperPosition', [0 0 1 1]);
hold off;
%print(gcf,'-bestfit','stepresponse_new', '-dpdf');
print(gcf,'stepresponse_new','-depsc');

figure(2);
p1(1) = plot(time,velocityOld,'DisplayName','Stepresponse gemessen');
hold on;
grid on;
p1(2)  = plot(dataSimulinkOld.ans(1,:),dataSimulinkOld.ans(2,:),'DisplayName','Step');
p1(3)  = plot(dataSimulinkOld.ans(1,:),dataSimulinkOld.ans(3,:),'DisplayName','Stepresponse Simulink');
p1(4) = plot(dataNewStep.stepnew.Zeit,dataNewStep.stepnew.istOld,'DisplayName','Stepresponse gemessen neu');
% Plot settling time point
riseTimePlot(time,velocityOld,1000,p1(1).Color,50);
%riseTimePlot(dataSimulinkOld.ans(1,:),dataSimulinkOld.ans(3,:));
riseTimePlot(double(dataNewStep.stepnew.Zeit),double(dataNewStep.stepnew.istOld),1000,p1(4).Color,-50);
ylabel({'Geschwindigkeit $v$ in RPM'});
xlabel({'Zeit $t$ in $s$'});
legend(p1,'Location','southoutside','Orientation','horizontal');
h=gcf;
set(h,'PaperOrientation','landscape');
set(h,'PaperUnits','normalized');
set(h,'PaperPosition', [0 0 1 1]);
hold off;
%print(gcf,'-bestfit','stepresponse_old', '-dpdf');
print(gcf,'stepresponse_old','-depsc');

figure(3);
p(1) = plot(rampMessung.stepRamp.zeit,rampMessung.stepRamp.soll,'DisplayName','Rampe soll');
hold on;
p(2) = plot(rampMessung.stepRamp.zeit,rampMessung.stepRamp.istOld,'DisplayName','Rampe ist alt');
p(3) = plot(rampMessung.stepRamp.zeit,rampMessung.stepRamp.istNew,'DisplayName','Rampe ist neu');
ylabel({'Geschwindigkeit $v$ in RPM'});
xlabel({'Zeit $t$ in $s$'});
legend(p,'Location','southoutside','Orientation','horizontal');
grid on;
drawMinMax(rampMessung.stepRamp.zeit,rampMessung.stepRamp.istNew,70,p(3).Color);
drawMinMax(rampMessung.stepRamp.zeit,rampMessung.stepRamp.istOld,-70,p(2).Color);
hold off;
%print(gcf,'-bestfit','rampresponse', '-dpdf');
print(gcf,'rampresponse','-depsc');

figure(4);
p4(1) = plot(rampMessung2.stepRamp.zeit,rampMessung2.stepRamp.soll,'DisplayName','Rampe soll');
hold on;
p4(2) = plot(rampMessung2.stepRamp.zeit,rampMessung2.stepRamp.istNew,'DisplayName','Rampe neu');
ylabel({'Geschwindigkeit $v$ in RPM'});
xlabel({'Zeit $t$ in $s$'});
legend(p4,'Location','southoutside','Orientation','horizontal');
grid on;
%drawMinMax(rampMessung2.stepRamp.zeit,rampMessung2.stepRamp.istNew,0,p(3).Color);
hold off;
%print(gcf,'-bestfit','rampresponse', '-dpdf');
print(gcf,'rampresponseNew','-depsc');

function riseTimePlot(time,yValue,max,color,xValMax)
offset = (time(2) - time(1));
stepResults = stepinfo(yValue,time,max,'RiseTimeLimits',[0.1 0.9]);
tRise= stepResults.RiseTime;
tSettle= stepResults.SettlingTime;
indexSettle = find(time >= tSettle,1,'first');
if tRise < 0.0999
    indexRiseT = find(yValue >= 1000*0.63,1,'first');
else
    indexRiseT = find(time >= tRise,1,'first');
end
xRiseT = time(indexRiseT);
yRiseT = yValue(indexRiseT);
xSettle = time(indexSettle);
ySittle = yValue(indexSettle);
% Plot settling time point
plot(xRiseT,yRiseT,'o','Color',color);
plot(xSettle,ySittle,'o','Color',color);
% plot([0 xSettle],[ySettle ySettle],'k-.');
% plot([xSettle xSettle],[0 ySettle],'k-.');
strmin = ['$T_{r}$: ',num2str(xRiseT-0.1)];
text(xRiseT+ offset*3,yRiseT+xValMax,strmin);
strmin = ['$T_{s}$: ',num2str(xSettle-0.1)];
text(xSettle+offset*3,ySittle+xValMax,strmin);
end

function drawMinMax(time,yValue,xValMax,color)
indexMin = find(time >= 1.75,1,'first');
indexMax = find(time >= 7,1,'first');

minValue = min(yValue(indexMin:indexMax));
indexA = find(minValue == yValue(indexMin:indexMax))+indexMin-1;

maxValue = max(yValue(indexMin:indexMax));
indexB = find(maxValue == yValue(indexMin:indexMax))+indexMin-1;
maxTime =  time(indexB);

offset = (time(2) - time(1))/length(time);

minTime =  time(indexA);
for i=1:1%length(minTime)   
plot(minTime(i),minValue,'o','Color',color);
strmin = ['Min.: ',num2str(minValue)];
text(minTime(i)-offset*20,minValue-60,strmin);
end
% plot([0 minTime],[minValue minValue],'k-.');
% plot([minTime minTime],[0 minValue],'k-.');

plot(maxTime,maxValue,'o','Color',color);
% plot([0 maxTime],[maxValue maxValue],'k-.');
% plot([maxTime maxTime],[0 maxValue],'k-.');

strmax = ['Max.: ',num2str(maxValue)];
text(maxTime-offset*20,maxValue+xValMax,strmax);
end
