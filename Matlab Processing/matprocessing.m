i = 1;
fileID = fopen('broject.txt', 'wt');
s1 = [0 0 0 0 0 0];
s2 = [0 0 0 0 0 0];
s3 = [0 0 0 0 0 0];
b1 = [0 0 0 0 0 0];
b2 = [0 0 0 0 0 0];
b3 = [0 0 0 0 0 0];
for m = 1:1:6
    count = 0;
    while ~strcmp(v1(i), 'Total')
        if v2(i) == 1
            if strcmp(v3(i), 'Blocked')
                b1(m) = b1(m) + 1;
            else
                s1(m) = s1(m) + 1;
            end
        end
        
        if v2(i) == 2
            if strcmp(v3(i), 'Blocked')
                b2(m) = b2(m) + 1;
            else
                s2(m) = s2(m) + 1;
            end
        end
        
        if v2(i) == 3
            if strcmp(v3(i), 'Blocked')
                b3(m) = b3(m) + 1;
            else
                s3(m) = s3(m) + 1;
            end
        end
        
        i = i + 1;
    end

TSM(m)=v6(i);
TBM(m)=v6(i+1);
TM=TSM+TBM;
i=i+12;
 % Write output to the file
    fprintf(fileID, 'Timer period=%d\n Counter=%d\n Sum Of Sent Messages Time for Sender1=%d\n Sum Of Sent Messages Time for Sender2=%d\n Sum Of Sent Messages Time for Sender3=%d\n The total number of successfully sent messages =%d, the total number of blocked messages=%d\n Blocked Messages for Sender 1= %d, Sent Messages Time for Sender 1= %d\n Blocked Messages for Sender 2= %d, Sent Messages Time for Sender 2= %d\n Blocked Messages for Sender 3=%d, Sent Messages Time for Sender 3=%d\n \n\n', m,i-12,v8(i-6),v8(i-4),v8(i-2), TSM(m), TBM(m), b1(m), s1(m), b2(m), s2(m), b3(m), s3(m));
end

fclose(fileID);
ts1=s1+b1;
ts2=s2+b2;
ts3=s3+b3;
a = [50 80 110 140 170 200];
b = [150, 200, 250, 300, 350, 400];
c = (a + b) / 2;
c1=[98.3,139.976,179.973,221.144,261.717,302.926];
c2=[98.96,138.978,180.5,220.24,260.161,302];
c3=[98.85,140.12,182.659,222.463,257.918,300.929];

ct = (c1 + c2 + c3)/3;
plot(ct,TM,'--gs', 'LineWidth',2,  'MarkerSize',10,  'MarkerEdgeColor','b',   'MarkerFaceColor',[0.5,0.5,0.5])
xlabel('Average sender timer period')
ylabel('the number of total sent messages')
title('the number of total Sent messages VS Average Sender timer period')

figure;
plot(ct,TBM,'--gs', 'LineWidth',2,  'MarkerSize',10,  'MarkerEdgeColor','b',   'MarkerFaceColor',[0.5,0.5,0.5])
xlabel('Average sender timer period')
ylabel('the number of total Blocked messages')
title('the number of total Blocked messages VS Average Sender timer period')

figure;
plot(c1,ts1,'--bs', 'LineWidth',2,  'MarkerSize',10,  'MarkerEdgeColor','b',   'MarkerFaceColor',[0.5,0.5,0.5])
hold on;
plot(c2,ts2,'--gs', 'LineWidth',2,  'MarkerSize',10,  'MarkerEdgeColor','g',   'MarkerFaceColor',[0.5,0.5,0.5])
plot(c3,ts3,'--rs', 'LineWidth',2,  'MarkerSize',10,  'MarkerEdgeColor','r',   'MarkerFaceColor',[0.5,0.5,0.5])
xlabel('Average sender timer period')
ylabel('the number of sent messages')
title('the number of sent messages for the high and low priority sender tasks')
legend ('Task 1 (low priority)','Task 2 (low priority)','Task 3 (high priority)')

figure;
plot(c1,b1,'--bs', 'LineWidth',2,  'MarkerSize',10,  'MarkerEdgeColor','b',   'MarkerFaceColor',[0.5,0.5,0.5])
hold on;
plot(c2,b2,'--gs', 'LineWidth',2,  'MarkerSize',10,  'MarkerEdgeColor','g',   'MarkerFaceColor',[0.5,0.5,0.5])
plot(c3,b3,'--rs', 'LineWidth',2,  'MarkerSize',10,  'MarkerEdgeColor','r',   'MarkerFaceColor',[0.5,0.5,0.5])
xlabel('Average sender timer period')
ylabel('the number of Blocked messages')
title('the number of Blocked messages for the high and low priority sender tasks')
legend ('Task 1 (low priority)','Task 2 (low priority)','Task 3 (high priority)')

figure;
plot(c1,s1,'--bs', 'LineWidth',2,  'MarkerSize',10,  'MarkerEdgeColor','b',   'MarkerFaceColor',[0.5,0.5,0.5])
hold on;
plot(c2,s2,'--gs', 'LineWidth',2,  'MarkerSize',10,  'MarkerEdgeColor','g',   'MarkerFaceColor',[0.5,0.5,0.5])
plot(c3,s3,'--rs', 'LineWidth',2,  'MarkerSize',10,  'MarkerEdgeColor','r',   'MarkerFaceColor',[0.5,0.5,0.5])
xlabel('Average sender timer period')
ylabel('the number of Successful sent messages')
title('the number of Successful sent messages for the high and low priority sender tasks')
legend ('Task 1 (low priority)','Task 2 (low priority)','Task 3 (high priority)')