#include "../myFunction.h"
#include "../myConst.h"
TF1* ftemp;
double funMultiplyPt(double* x, double* par) {
    return ftemp->Eval(x[0]) * 2.* TMath::Pi() * x[0];
}
void plot_Rcp_fit1() {
    globalSetting();
    char dir[250];
    char name[250];
    char title[250];
    //TString CMD = 0;
    char CMD[250];
    TLegend* legend;
    TH1F* h0;
    const float PI = TMath::Pi();
    
    sprintf(dir,"pic");
    sprintf(CMD,"[ -d %s ] || mkdir -p %s",dir,dir);
    gSystem->Exec(CMD);
    
    //change the base line--last cent bin
    const int ncent = 5;
    const char nameCent[ncent][250] = {"0-10%", "10-20%", "20-40%", "40-60%", "60-80%"};
    const char nameCentXL[ncent][250] = {"0_10", "10_20", "20_40", "40_60", "60_80"};
    float NbinMean[ncent] = {938.80170, 579.89409, 288.35051, 91.37100, 21.37396};
    
    const int npt = 10;
    const double nptbin[npt+1] = { 0., 0.5, 1., 1.5, 2., 2.5, 3., 4.0, 5.0, 6.0, 8.0 };//
    
    //Read spectra
    TGraphErrors* gD0err_xl[ncent];
    TGraphErrors* gD0sys_xl[ncent];
    TF1* fLevy[ncent];
    TFile* fin1 = new TFile("../ptShift/D0_Spectra_Run14HFT.root");
    for(int icent=0; icent<ncent; icent++) {
        gD0err_xl[icent] = (TGraphErrors*)fin1->Get(Form("gD0_err_%s",nameCentXL[icent]));
        gD0sys_xl[icent] = (TGraphErrors*)fin1->Get(Form("gD0_sys_%s",nameCentXL[icent]));
        fLevy[icent] = (TF1*)fin1->Get(Form("flevy_%s",nameCentXL[icent]));
    }
    fin1->Close();
    
    //calculate Rcp
    /*for(int icent=0; icent<ncent; icent++) {
        cout << nameCent[icent] << endl;
        for(int ipt=0; ipt<gD0err_xl[0]->GetN(); ipt++) {
            float pt = gD0err_xl[icent]->GetX()[ipt];
            float y = gD0err_xl[icent]->GetY()[ipt]/NbinMean[icent];
            float yErr = gD0err_xl[icent]->GetEY()[ipt]/NbinMean[icent];
            float base = fLevy[ncent-1]->Eval(pt)/NbinMean[ncent-1];//gD0err_xl[ncent-1]->GetY()[ipt]/NbinMean[ncent-1];  //60-80%
            float baseErr = 0;//gD0err_xl[ncent-1]->GetEY()[ipt]/NbinMean[ncent-1];
            float Rcp = y/base;
            float RcpErr = Rcp*sqrt(pow(yErr/y,2)+pow(baseErr/base,2));
            gD0err_xl[icent]->GetY()[ipt] = Rcp;
            gD0err_xl[icent]->GetEY()[ipt] = RcpErr;
            cout << gD0err_xl[icent]->GetX()[ipt] << "\t" << Rcp << "\t" << RcpErr << endl;
        }
        cout << endl;
    }*/
    //calculate Rcp -- integral
    ftemp = (TF1*)fLevy[ncent-1]->Clone("fbaseClone");
    TF1* fBase = new TF1("fppbaseI",funMultiplyPt,0,10,0);
    for(int icent=0; icent<ncent; icent++) {
        cout << nameCent[icent] << endl;
        for(int ipt=0; ipt<gD0err_xl[0]->GetN(); ipt++) {
            float pt = gD0err_xl[icent]->GetX()[ipt];
            float y = gD0err_xl[icent]->GetY()[ipt]/NbinMean[icent];
            float yErr = gD0err_xl[icent]->GetEY()[ipt]/NbinMean[icent];
            float ptW = nptbin[ipt+1] - nptbin[ipt];
            float ptM = (nptbin[ipt+1] + nptbin[ipt])/2.;
            y = y*ptM*ptW* 2.* TMath::Pi();
            yErr = yErr*ptM*ptW* 2.* TMath::Pi();
            float base = fBase->Integral(nptbin[ipt],nptbin[ipt+1])/NbinMean[ncent-1];//gD0err_xl[ncent-1]->GetY()[ipt]/NbinMean[ncent-1];  //60-80%
            float baseErr = 0;//gD0err_xl[ncent-1]->GetEY()[ipt]/NbinMean[ncent-1];
            float Rcp = y/base;
            float RcpErr = Rcp*sqrt(pow(yErr/y,2)+pow(baseErr/base,2));
            gD0err_xl[icent]->GetY()[ipt] = Rcp;
            gD0err_xl[icent]->GetEY()[ipt] = RcpErr;
            cout << gD0err_xl[icent]->GetX()[ipt] << "\t" << Rcp << "\t" << RcpErr << endl;
        }
        cout << endl;
    }
    
    //set for plot
    float markerSize = 2.0;
    float lineWidth = 2;
    for(int icent=0; icent<ncent; icent++) {
        gD0err_xl[icent]->SetMarkerStyle(kFullCircle);
        gD0err_xl[icent]->SetMarkerSize(markerSize);
        gD0err_xl[icent]->SetMarkerColor(COLOR[icent]);
        gD0err_xl[icent]->SetLineWidth(lineWidth);
        gD0err_xl[icent]->SetLineColor(COLOR[icent]);
        
        //fLevy[icent]->SetLineColor(COLOR[icent]);
        //fLevy[icent]->SetLineWidth(lineWidth);
    }
    
    //plot Rcp
    TCanvas* c1 = new TCanvas("c1", "A Canvas",10,10,800,800);
    setPad(c1);
    float ymin = 0;
    float ymax = 1.5;
    h0= new TH1F("","",1,0,8);
    h0->Draw();
    h0->SetMinimum(ymin),
    h0->SetMaximum(ymax);
    setHisto(h0,"","p_{T} (GeV/c)", Form("R_{cp} (/%s)",nameCent[ncent-1]));
    const float legy_lw = 0.93 - 0.05*(ncent-1);
    legend = new TLegend(0.7,legy_lw,0.9,0.93);
    legend->SetFillStyle(0);
    legend->SetFillColor(10);
    legend->SetBorderSize(0);
    legend->SetTextSize(0.04);
    legend->SetTextFont(132);
    //legend->SetHeader("Guannan");
    for(int icent=0; icent<ncent-1; icent++) {
        legend->AddEntry(gD0err_xl[icent],nameCent[icent],"p");
        gD0err_xl[icent]->Draw("psame");
    }
    legend->Draw();
    sprintf(name,"%s/D0_Rcp1_fit_Integral.pdf",dir);
    c1->SaveAs(name);
}
