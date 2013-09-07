
%cd 'D:\data\sectorialnondominance\代码\MOEA-D-DE\mfile'
% nkp = 2;
% nitems = 250;
% nbreak = 10;%10;

%P22D1L22   P21D1L21 P21D1L22 P21D1L26 DMOEA SNGA
FilePath4 = 'front.txt';
    path = FilePath4;%[FilePath4, int2str(1), '.txt'];
    [x11, x12]=textread(path, '%f %f');
%    [x11, x12]=sorting([x11, x12]);


    
    figure(2);
    %plot( x11,x12,':sc', x21,x22,':og', x31,x32,':*r', x41,x42,':.b',x51,x52,':.b',x61,x62,':.b',x71,x72,':.b',x81,x82,':.b',x91,x92,':.b',x101,x102,':.b');
    plot( x11,x12,'ob');%'sc' 'og' x41,x42,'oc',   , x31,x32,'og', x51,x52,'.r'
    %hold on; 
    %plot(100,100,'*r');

    set(gca,'FontSize',8,'FontName','Arial');
    %legend('gen=100','gen=200','gen=300','gen=400','gen=500','gen=600','gen=700','gen=800','gen=900','gen=1000',10);
    %legend('NSGA-II所求Pareto最优解集',1);%'gen=500',    ,'gen=50','gen=200','gen=1000'
    title('NSGA-II所求Pareto最优解集');
    xlabel('f_1');
    ylabel('f_2');




