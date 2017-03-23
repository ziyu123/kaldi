#!/usr/bin/env python
# Copyright 2016  Tsinghua University
#                 (Author: Yixiang Chen, Lantian Li, Dong Wang)
# Licence: Apache 2.0


import sys
from math import *


# Load the result file in the following format
# <lang-id> ct-cn     id-id     ja-jf     ko-kr     ru-ru     vi-vn     zh-cn
# <utt-id>  <score1>  <score2>  <score3>  <score4>  <score5>  <score6>  <score7>

# The language identity is defined as: 
# 1: ct-cn, 2: id-id, 3: ja-jf, 4: ko-kr, 5: ru-ru, 6: vi-vn, 7: zh-cn


dictl = {'c':1, 'i':2}

# Load scoring file and label.scp.
def Loaddata(fin, labelscp):

    x = []
    for i in range(8):
        x.append(0)
    fin = open(fin, 'r')
    lines = fin.readlines()
    fin.close()

    labelscp = open(labelscp, 'r')
    linesw = labelscp.readlines()
    labelscp.close()

    labelscpdict = {}
    for line in linesw:
        part = line.split()
        labelscpdict[part[0].split('.')[0]] = part[1]

    label = []
    part = lines[0].split()
    for i in range(7):
        label.append(dictl[part[i][0]])

    data = []
    for line in lines[1:]:
        part = line.split()
        x[0] = labelscpdict[part[0].split('.')[0]]
        for i in range(7):
            x[label[i]] = part[i + 1]
        data.append(x)
        x = []
        for i in range(8):
            x.append(0)

    datas = []
    for ll in data:
        for lb in range(7):
            datas.append([dictl[ll[0][0]], lb + 1, float(ll[lb + 1])])    
            

    # score normalized to [0, 1] 
    for i in range(len(datas) / 7):
        sum = 0
        for j in range(7):
            k = i * 7 + j
            sum += exp(datas[k][2])
        for j in range(7):
            k = i * 7 + j
            datas[k][2] = exp(datas[k][2]) / sum


    return datas

# Compute Cavg.
# data: matrix for result scores, assumed within [0,1].
# sn: number of bins in Cavg calculation.
def CountCavg(data, sn = 20, lgn = 7):

    Cavg = [0.0] * (sn + 1) 
    # Cavg: Every element is the Cavg of the corresponding precision
    precision = 1.0 / sn
    for section in range(sn + 1):
        threshold = section * precision
        target_Cavg = [0.0] * lgn
        # target_Cavg: P_Target * P_Miss + sum(P_NonTarget*P_FA)

        for language in range(lgn):
            P_FA = [0.0] * lgn
            P_Miss = 0.0
            # compute P_FA and P_Miss
            LTm = 0.0; LTs = 0.0; LNm = 0.0; LNs = [0.0] * lgn;
            for line in data:
                language_label = language + 1
                if line[0] == language_label:
                    if line[1] == language_label:
                        LTm += 1
                        if line[2] < threshold:
                            LTs += 1
                    for t in range(lgn):
                        if not t == language:
                            if line[1] == t + 1:
                                if line[2] > threshold:
                                    LNs[t] += 1
            LNm = LTm
            for Ln in range(lgn):
                P_FA[Ln] = LNs[Ln] / LNm
                
            P_Miss = LTs / LTm
            P_NonTarget = 0.5 / (lgn - 1)
            P_Target = 0.5
            target_Cavg[language] = P_Target * P_Miss + P_NonTarget*sum(P_FA)

        for language in range(lgn):
            Cavg[section] += target_Cavg[language] / lgn   
            
    return Cavg, min(Cavg)


if __name__ == '__main__':

    if (len(sys.argv) != 3):
        print "usage %s <result file path> <label file path>" % (sys.argv[0])
        exit(0)
    
    data = Loaddata(sys.argv[1], sys.argv[2])
    
    # default precision as 20 bins, 7 languages
    cavg, mincavg = CountCavg(data, 20, 7)

    print "Cavg is :" + str(cavg)
    print "Minimal Cavg is :" + str(mincavg)



