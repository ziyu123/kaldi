%------------------------------
% load data
target = load('target.txt');
nontarget = load('nontarget.txt');

%------------------------------
%initialize the DCF parameters
Set_DCF (10, 1, 0.01);

%------------------------------
%compute Pmiss and Pfa from experimental detection output scores
[P_miss_1,P_fa_1] = Compute_DET (target, nontarget);

%------------------------------
%plot results

% Set tic marks
Pmiss_min = 0.001;
Pmiss_max = 0.60;
Pfa_min = 0.001;
Pfa_max = 0.60;
Set_DET_limits(Pmiss_min, Pmiss_max, Pfa_min, Pfa_max);

%call figure, plot DET-curve
figure;
Plot_DET (P_miss_1, P_fa_1,'--r',3);
hold on;


%find lowest cost point and plot
C_miss = 1;
C_fa = 1;
P_target = 0.5;
Set_DCF(C_miss, C_fa, P_target);

[DCF_opt_1, Popt_miss_1, Popt_fa_1] = Min_DCF(P_miss_1, P_fa_1);
fprintf('minDCF is: %.4f \n', DCF_opt_1)
Plot_DET(Popt_miss_1, Popt_fa_1, 'ko', 2);
hold on;

z = abs(P_miss_1 - P_fa_1);
[ed, site] = min(z);

fprintf('EER is: %.4f %% \n', 100 * P_miss_1(site))
Plot_DET(P_fa_1(site), P_fa_1(site), 'bd', 2);
hold on;
